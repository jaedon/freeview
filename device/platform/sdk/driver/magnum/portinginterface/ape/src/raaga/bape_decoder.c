/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_decoder.c $
 * $brcm_Revision: Hydra_Software_Devel/SW7425-5116-12.4/1 $
 * $brcm_Date: 12/6/13 11:57a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_decoder.c $
 * 
 * Hydra_Software_Devel/SW7425-5116-12.4/1   12/6/13 11:57a mpeteri
 * SW7425-5116: back-port changes to 12.4
 * 
 * Hydra_Software_Devel/SW7425-5116/1   12/6/13 11:27a mpeteri
 * SW7425-5116: changes to allow 96k output rate from the decoder
 * 
 * Hydra_Software_Devel/99   10/7/12 8:19p jgarrett
 * SW7346-914: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7346-914/3   10/7/12 6:50p jgarrett
 * SW7346-914: Initial working version of ancillary data
 * 
 * Hydra_Software_Devel/SW7346-914/2   10/3/12 3:54a jgarrett
 * SW7346-914: Initializing ancillary data structure prior to start and
 * enabling configuration
 * 
 * Hydra_Software_Devel/SW7346-914/1   9/28/12 12:05p jgarrett
 * SW7346-914: Initial ancillary data implementation
 * 
 * Hydra_Software_Devel/98   7/20/12 4:43p jgarrett
 * SW7425-3490: Merge to main branch
 * 
 * Hydra_Software_Devel/97   7/16/12 3:27p jgarrett
 * SW7231-903: Converting sample rates > 48k to a base rate for
 * passthrough
 * 
 * Hydra_Software_Devel/SW7425-3490/1   7/13/12 5:41p jgarrett
 * SW7425-3490: Adding initial frame preservation for MS-11 certification
 * 
 * Hydra_Software_Devel/96   6/26/12 12:53p jgarrett
 * SW7358-343: Enabling PPM correction for DSOLA modes
 * 
 * Hydra_Software_Devel/95   6/6/12 3:24p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * Hydra_Software_Devel/95   6/6/12 2:44p mpeteri
 * SW7425-2889: Changes for DTS-CD support
 * 
 * Hydra_Software_Devel/SW7425-2889/1   6/5/12 4:30p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * Hydra_Software_Devel/94   6/1/12 5:17p jgarrett
 * SW7425-2075: Not attempting to set passthrough config for MLP
 * 
 * Hydra_Software_Devel/93   5/30/12 9:54a jgarrett
 * SW7425-3129: Allowing WMA/AAC status to read from stream info if the
 * mode change interrupt has not fired
 * 
 * Hydra_Software_Devel/92   5/23/12 2:42p jgarrett
 * SW7425-2075: Forcing PPM correction off when mixing with the DSP
 * 
 * Hydra_Software_Devel/91   5/15/12 10:10a jgarrett
 * SW7425-3065: Changing LPCM-BD to use LPCM-DVD algorithm with framesync
 * option
 * 
 * Hydra_Software_Devel/90   5/7/12 3:12p gskerl
 * SW7425-2075: Fixed BAPE_Decoder_GetStatus() so that it now checks for a
 * valid inputPort before attempting to dereference it.
 * 
 * Hydra_Software_Devel/89   5/4/12 5:59p gskerl
 * SW7425-2075: Consolidated redundant halted flags from BAPE_MaiInput
 * struct and BAPE_Decoder struct into the BAPE_InputPortObject struct.
 * 
 * Hydra_Software_Devel/88   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/7   5/3/12 3:43p gskerl
 * SW7425-2075: Added check for input sample rates higher than 48K when
 * starting decoder.
 * 
 * Hydra_Software_Devel/SW7425-2075/6   5/3/12 3:13p gskerl
 * SW7425-2075: Added code to handle the BAPE_Decoder struct's new halted
 * and running fields. Also PCM input sample rates higher than 48K will
 * halt the decoder (because they're not supported).
 * 
 * Hydra_Software_Devel/SW7425-2075/5   4/16/12 3:47p jgarrett
 * SW7425-2075: Fixing compressed 4x/16x handling
 * 
 * Hydra_Software_Devel/SW7425-2075/4   4/12/12 1:55p jgarrett
 * SW7425-2075: Revising InputPort format handling
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/11/12 3:22p gskerl
 * SW7425-2075: Change Decoder's allowed BAPE_DataSource from eDfifo to
 * eFci.
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/6/12 3:51p jgarrett
 * SW7425-2075: Adding debug, fixing buffer logic
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:49p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/87   3/8/12 5:08p gskerl
 * SW7429-18: Rearranged code so that BAPE_InputPort_P_AttachConsumer() is
 * called before accessing the InputPort format fields.
 * 
 * Hydra_Software_Devel/86   3/8/12 10:56a jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/85   3/7/12 4:35p jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/84   2/23/12 4:26p gskerl
 * SW7429-18: Added 7429 SPDIF input support. Use saved audio format from
 * handle instead of from PathConnector.
 *
 * Hydra_Software_Devel/84   2/23/12 4:21p gskerl
 * SW7429-18: Added 7429 SPDIF input support. Use saved audio format from
 * handle instead of from PathConnector.
 * 
 * Hydra_Software_Devel/83   2/21/12 6:41p jgarrett
 * SW7231-363: Correctly recovering if StartTask fails.  Propagating
 * compressed connector format properly
 * 
 * Hydra_Software_Devel/82   2/17/12 4:13p jgarrett
 * SW7425-2262: Adding API to enter Non-Realtime Gap Fill mode
 * 
 * Hydra_Software_Devel/81   2/9/12 5:44p jgarrett
 * SW7425-2074: Failing gracefully if PCM 5.1/7.1 is requested as input
 * for HDMI
 * 
 * Hydra_Software_Devel/80   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/79   2/3/12 4:29p jgarrett
 * SW7425-2268: Initial bringup of voice conferencing support
 * 
 * Hydra_Software_Devel/78   2/2/12 12:14p gskerl
 * SW7429-18: Added BAPE_Decoder_P_InputFormatChange_isr() callback to
 * stop decoding when the input port detects a format change.
 * 
 * Hydra_Software_Devel/77   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/76   1/13/12 3:12p gskerl
 * SW7429-18: Added 7429 support for MAI input.
 * 
 * Hydra_Software_Devel/75   12/29/11 1:14p jgarrett
 * SW7231-500: Adding IEEE-1394 LPCM
 * 
 * Hydra_Software_Devel/74   12/19/11 4:27p jgarrett
 * SW7425-1018: Adding initial A/85 implementation
 * 
 * Hydra_Software_Devel/73   12/15/11 3:29p jgarrett
 * SW7425-1756: Disabling status ready interrupt after it fires once.
 * 
 * Hydra_Software_Devel/72   12/15/11 3:22p jgarrett
 * SW7231-500: Converting framesync settings from struct to union
 * 
 * Hydra_Software_Devel/71   12/9/11 5:50p jgarrett
 * SW7425-1756: Adding debug and correcting some ISR callbacks
 * 
 * Hydra_Software_Devel/70   11/21/11 11:44a jgarrett
 * SW7425-1756: Adding overflow and status ready interrupts
 * 
 * Hydra_Software_Devel/69   11/14/11 3:40p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/68   9/27/11 5:36p jgarrett
 * SW7425-1352: Adding STCValid debug prints
 * 
 * Hydra_Software_Devel/67   9/15/11 9:24a jgarrett
 * SW7425-1285: Sending lower 32-bits of STC to DSP on systems with 42-bit
 * STC support
 * 
 * Hydra_Software_Devel/66   9/14/11 6:58p jgarrett
 * SW7425-1045: Adding unmapped settings and status values for some
 * decoder algorithms
 * 
 * Hydra_Software_Devel/65   9/8/11 10:42a jgarrett
 * SWDTV-6627: Adding BAPE_Equalizer_GetDefaultSettings stub for chips
 * w/out EQ support
 * 
 * Hydra_Software_Devel/64   8/25/11 12:20p jgarrett
 * SW7231-295: Propagating PPM correction setting to DSP Task
 * 
 * Hydra_Software_Devel/63   8/22/11 7:03p jgarrett
 * SWDTV-8271: Adding low delay mode support
 * 
 * Hydra_Software_Devel/61   8/12/11 7:05p sgadara
 * SWDTV-6584: [35233] Reorganize Input port FCI id extraction
 * 
 * Hydra_Software_Devel/60   8/12/11 4:19p venkatr
 * SWDTV-6584 : [35233] ADC input code cleanup
 * 
 * Hydra_Software_Devel/59   8/11/11 6:24p jgarrett
 * SWDTV-6305: Setting RF I2S input as I2S 1
 * 
 * Hydra_Software_Devel/58   8/10/11 9:29a venkatr
 * SWDTV-6584 : [35233] Add ADC Input for APE
 * 
 * Hydra_Software_Devel/57   8/8/11 6:01p jgarrett
 * SW7346-412: Marking PTS as invalid if DSP is not ready
 * 
 * Hydra_Software_Devel/56   7/18/11 4:13p jgarrett
 * SW7425-910: Correcting passthrough FMM programming for LSF/QSF sources
 * 
 * Hydra_Software_Devel/55   7/11/11 6:57p jgarrett
 * SW7425-64: Updating sample rate handling for new callback
 * 
 * Hydra_Software_Devel/54   7/8/11 4:52p gskerl
 * SW7425-321: Added printing of downstream nodes when an audio path is
 * started (if bape_startprint is enabled)
 * 
 * Hydra_Software_Devel/53   7/6/11 4:55p jgarrett
 * SW7358-62: Storing a local copy of the XptContextMap structure
 * 
 * Hydra_Software_Devel/52   7/1/11 9:59a jgarrett
 * SW7405-5072: Adding WMA TS Support
 * 
 * Hydra_Software_Devel/51   6/29/11 5:00p jgarrett
 * SW7425-620: Allowing multichannel/compressed outputs from a decoder to
 * override to PCM if the codec does not support multichannel/compressed
 * output
 * 
 * Hydra_Software_Devel/50   6/28/11 11:08a jgarrett
 * SW7425-654: Adding AAC passthrough remux configuration
 * 
 * Hydra_Software_Devel/49   6/27/11 5:41p jgarrett
 * SW7231-97: Refactoring SPDIF/HDMI enable mechanisms to handle older
 * Onkyo receiver DTS->PCM switching requirements
 * 
 * Hydra_Software_Devel/48   6/27/11 8:27a jgarrett
 * SW7425-654: Merging NRT to main line
 * 
 * Hydra_Software_Devel/NRT_XCODE_DEVEL/1   6/21/11 5:54p jgarrett
 * SW7425-654: Adding NRT hooks
 * 
 * Hydra_Software_Devel/47   6/21/11 3:50p jgarrett
 * SW7425-654: Adding NRT APIs to decoder and mux
 * 
 * Hydra_Software_Devel/46   6/20/11 3:58p jgarrett
 * SW7425-408: Adding MS usage case restriction regarding AAC-HE simul
 * passthrough due to sample rate conversion in the decoder
 * 
 * Hydra_Software_Devel/45   6/16/11 6:40p jgarrett
 * SW7422-456: Always applying TSM settings even if task is not started
 * yet.
 * 
 * Hydra_Software_Devel/44   6/16/11 11:01a gskerl
 * SW7425-321: Added call to BAPE_Mixer_P_PrintMixers() just before
 * starting paths
 * 
 * Hydra_Software_Devel/43   5/27/11 4:56p jgarrett
 * SW7425-408: Propagating decoder settings for AAC/AC3 to DDRE in MS11
 * usage modes
 * 
 * Hydra_Software_Devel/42   5/26/11 6:10p jgarrett
 * SW7425-408: Making generic passthru optional
 * 
 * Hydra_Software_Devel/41   5/5/11 5:52p jgarrett
 * SW7425-522: Returning error on unsupported trick rates
 * 
 * Hydra_Software_Devel/40   4/27/11 6:10p jgarrett
 * SW7344-48: Flipping CDB LittleEndian flag in Big Endian
 * 
 * Hydra_Software_Devel/39   4/19/11 5:33p jgarrett
 * SW7422-408: Adding additional codecs
 * 
 * Hydra_Software_Devel/38   4/19/11 11:05a jgarrett
 * SW7344-46: Fixing multichannel support test
 * 
 * Hydra_Software_Devel/37   4/18/11 10:09p jgarrett
 * SW7425-361: Refactoring DSP branch decisions
 * 
 * Hydra_Software_Devel/36   4/17/11 1:55p jgarrett
 * SW7425-288: Adding audio codec to metadata
 * 
 * Hydra_Software_Devel/35   4/17/11 12:28p jgarrett
 * SW7344-46: Adding error checking for license issues and attempts to
 * enable unsupported features
 * 
 * Hydra_Software_Devel/34   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/33   4/6/11 1:23a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:49p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/32   3/27/11 12:19p piyushg
 * SW7422-364: Reading the correct MAI FORMAT register.
 * 
 * Hydra_Software_Devel/31   3/24/11 5:25p jgarrett
 * SW7422-146: Fixes for kernel mode and big endian
 * 
 * Hydra_Software_Devel/30   3/23/11 4:29p jgarrett
 * SW7422-403: Removing error return when dsola is requested with
 * compressed/multichannel outputs
 * 
 * Hydra_Software_Devel/30   3/23/11 3:48p jgarrett
 * SW7422-403: Not failing for DSOLA request with Compressed/Multichannel
 * out
 * 
 * Hydra_Software_Devel/29   3/23/11 11:04a piyushg
 * SW7422-146: Fixing compilation errors.
 * 
 * Hydra_Software_Devel/28   3/22/11 6:27p jgarrett
 * SW7422-401: Fixing implementation of DisableForFlush and Flush
 * 
 * Hydra_Software_Devel/27   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/26   3/18/11 6:12p gskerl
 * SW7422-146: Changed audio input connector callbacks to take the connect
 * handle as an argument
 * 
 * Hydra_Software_Devel/26   3/18/11 6:04p gskerl
 * SW7422-146: Changed audio input connector callbacks to take the connect
 * handle as an argument
 * 
 * Hydra_Software_Devel/25   3/14/11 6:03p jgarrett
 * SW7422-146: Disabling PPM correction for external input sources
 * 
 * Hydra_Software_Devel/24   3/11/11 6:02p jgarrett
 * SW7422-146: Decoder supports external inputs
 * 
 * Hydra_Software_Devel/23   3/11/11 12:20p jgarrett
 * SW7422-146: Enabling PPM Correction
 * 
 * Hydra_Software_Devel/22   3/9/11 2:54p jgarrett
 * SW7422-146: Fixing SRC lockup with decode stop/start
 * 
 * Hydra_Software_Devel/21   3/3/11 6:31p jgarrett
 * SW7422-146: Adding SRC and DSOLA and path review feedback
 * 
 * Hydra_Software_Devel/20   3/1/11 5:13p jgarrett
 * SW7422-146: Fixing post-processing output modes
 * 
 * Hydra_Software_Devel/19   2/28/11 5:15p jgarrett
 * SW7422-146: Fixing passthrough config for DDP
 * 
 * Hydra_Software_Devel/18   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/17   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/16   2/16/11 3:04p jgarrett
 * SW7422-146: Coverity CID 255
 * 
 * Hydra_Software_Devel/15   2/15/11 4:07p jgarrett
 * SW7422-146: Adding additional codec settings and types
 * 
 * Hydra_Software_Devel/14   2/7/11 11:30a jgarrett
 * SW7422-146: Implementing DDP -> AC3 conversion and status for
 * MPEG/AAC/AC3
 * 
 * Hydra_Software_Devel/13   2/2/11 6:06p jgarrett
 * SW7422-146: Fixing arguments to simul branches
 * 
 * Hydra_Software_Devel/12   2/2/11 4:01p jgarrett
 * SW7422-146: Switching converter data type to auxiliary
 * 
 * Hydra_Software_Devel/11   2/2/11 2:17p jgarrett
 * SW7422-146: Adding decoder status placeholder
 * 
 * Hydra_Software_Devel/10   2/1/11 5:41p jgarrett
 * SW7422-146: Added MPEG support
 * 
 * Hydra_Software_Devel/9   2/1/11 5:01p jgarrett
 * SW7422-146: Adding AAC and Generic Passthrough
 * 
 * Hydra_Software_Devel/8   1/28/11 3:38p jgarrett
 * SW7422-146: Adding support for simul handling of DDP/WMA-Pro/Generic
 * 
 * Hydra_Software_Devel/7   1/27/11 5:47p jgarrett
 * SW7422-146: Adding support for multichannel and ac3 user parameters
 * 
 * Hydra_Software_Devel/6   1/19/11 6:50p jgarrett
 * SW7422-146: Fixing computation of number of source paths
 * 
 * Hydra_Software_Devel/5   1/19/11 6:13p jgarrett
 * SW7422-146: Enabling TSM
 * 
 * Hydra_Software_Devel/4   1/19/11 2:58p jgarrett
 * SW7422-146: Initial decode/passthrough of ac3
 * 
 * Hydra_Software_Devel/3   1/12/11 4:24p jgarrett
 * SW7422-146: Adding additional APIs
 * 
 * Hydra_Software_Devel/2   12/17/10 3:58p jgarrett
 * SW7422-146: Nexus APE integration on 7422
 * 
 * Hydra_Software_Devel/1   12/16/10 4:04p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#include "bchp_aud_fmm_bf_ctrl.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_decoder);

BDBG_OBJECT_ID(BAPE_Decoder);

#define BAPE_DISABLE_DSP 0  /* Enable this to check for CIT errors and avoid starting the DSP */

static void BAPE_Decoder_P_SetupDefaults(BAPE_DecoderHandle handle);
static void BAPE_Decoder_P_ConvertTsmStatus_isr(BAPE_DecoderTsmStatus *pStatus, const BDSP_AudioTaskTsmStatus *pDspStatus);
static void BAPE_Decoder_P_FirstPts_isr(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus);
static void BAPE_Decoder_P_TsmFail_isr(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus);
static void BAPE_Decoder_P_TsmPass_isr(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus);
static void BAPE_Decoder_P_SampleRateChange_isr(void *pParam1, int param2, unsigned streamSampleRate, unsigned baseSampleRate);
static void BAPE_Decoder_P_ModeChange_isr(void *pParam1, int param2, unsigned mode);
static void BAPE_Decoder_P_BitrateChange_isr(void *pParam1, int param2, const BDSP_AudioBitRateChangeInfo *pInfo);
static void BAPE_Decoder_P_Overflow_isr(void *pParam1, int param2);
static void BAPE_Decoder_P_StatusReady_isr(void *pParam1, int param2);
static BERR_Code BAPE_Decoder_P_InputFormatChange_isr(BAPE_PathNode *pNode,BAPE_InputPort inputPort);
static void BAPE_Decoder_P_CleanupTaskCreateSetings(BAPE_DecoderHandle handle);
static BERR_Code BAPE_Decoder_P_ApplyDsolaSettings(BAPE_DecoderHandle handle);
static BERR_Code BAPE_Decoder_P_ApplyFramesyncSettings(BAPE_DecoderHandle handle);
static BERR_Code BAPE_Decoder_P_Start(BAPE_DecoderHandle handle);
static void BAPE_Decoder_P_Stop(BAPE_DecoderHandle handle);
static bool BAPE_Decoder_P_TaskValid_isr(BAPE_DecoderHandle handle);
static BERR_Code BAPE_Decoder_P_DeriveMultistreamLinkage(BAPE_DecoderHandle handle);

void BAPE_Decoder_GetDefaultOpenSettings(
    BAPE_DecoderOpenSettings *pSettings     /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

BERR_Code BAPE_Decoder_Open(
    BAPE_Handle deviceHandle,
    unsigned index,
    const BAPE_DecoderOpenSettings *pSettings, 
    BAPE_DecoderHandle *pHandle                 /* [out] */
    )
{
    BAPE_DecoderOpenSettings defaults;
    BAPE_DecoderHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);

    if ( NULL == pSettings )
    {
        BAPE_Decoder_GetDefaultOpenSettings(&defaults);
        pSettings = &defaults;
    }

    if ( index >= BAPE_CHIP_MAX_DECODERS )
    {
        BDBG_ERR(("This chip only supports %u decoders.  Cannot open decoder %u", BAPE_CHIP_MAX_DECODERS, index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( deviceHandle->decoders[index] )
    {
        BDBG_ERR(("Decoder %d already open", index));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( pSettings->dspIndex >= deviceHandle->numDsps )
    {
        BDBG_ERR(("DSP %u is not available.  This system has %u DSPs.", pSettings->dspIndex, deviceHandle->numDsps));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    handle = BKNI_Malloc(sizeof(BAPE_Decoder));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_Decoder));
    BDBG_OBJECT_SET(handle, BAPE_Decoder);
    handle->deviceHandle = deviceHandle;
    handle->index = index;
    handle->dspIndex = pSettings->dspIndex;
    BKNI_Snprintf(handle->name, sizeof(handle->name), "Decoder %u", index);
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_eDecoder, 0, BAPE_ConnectorFormat_eMax, deviceHandle, handle);
    handle->node.pName = handle->name;
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].pName = "stereo";
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].useBufferPool = true;
    handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].pName = "multichannel";
    handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].useBufferPool = true;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed].pName = "compressed";
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed].useBufferPool = true;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x].pName = "compressed 4x";
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x].useBufferPool = true;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x].pName = "compressed 16x";
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x].useBufferPool = true;
    handle->node.connectors[BAPE_ConnectorFormat_eMono].pName = "mono";
    handle->node.connectors[BAPE_ConnectorFormat_eMono].useBufferPool = true;

    BAPE_FMT_P_InitDescriptor(&format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmStereo;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    format.type = BAPE_DataType_ePcm5_1;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    format.type = BAPE_DataType_eIec61937;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    format.type = BAPE_DataType_eIec61937x4;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    format.type = BAPE_DataType_eIec61937x16;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    format.type = BAPE_DataType_ePcmMono;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eMono], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    BAPE_PathNode_P_GetInputCapabilities(&handle->node, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eFci);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x4);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_eIec61937x16);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_caps; }

    /* Setup node callbacks */
    handle->node.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->node.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->node.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->node.inputPortFormatChange_isr = BAPE_Decoder_P_InputFormatChange_isr;
    handle->state = BAPE_DecoderState_eStopped;
    handle->settings.multichannelFormat = BAPE_MultichannelFormat_e5_1;
    handle->settings.dualMonoMode = BAPE_DualMonoMode_eStereo;
    handle->settings.outputMode = BAPE_ChannelMode_e3_2;
    handle->settings.outputLfe = true;
    handle->settings.decodeRate = BAPE_NORMAL_DECODE_RATE;
    handle->settings.loudnessEquivalenceEnabled = true;
    BAPE_Decoder_GetDefaultStartSettings(&handle->startSettings);
    BAPE_Decoder_P_SetupDefaults(handle);

    if ( pSettings->ancillaryDataFifoSize > 0 )
    {
        handle->pAncDataBufferDescriptor = BMEM_Heap_Alloc(deviceHandle->memHandle, sizeof(BDSP_AF_P_sDRAM_CIRCULAR_BUFFER));
        if ( NULL == handle->pAncDataBufferDescriptor )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto err_ancillary_buffer;
        }
        handle->pAncDataDspBuffer = BMEM_Heap_Alloc(deviceHandle->memHandle, pSettings->ancillaryDataFifoSize);
        if ( NULL == handle->pAncDataDspBuffer )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto err_ancillary_buffer;
        }
        BKNI_Memset(handle->pAncDataDspBuffer, 0xcd, pSettings->ancillaryDataFifoSize);
        handle->pAncDataHostBuffer = BMEM_Heap_Alloc(deviceHandle->memHandle, pSettings->ancillaryDataFifoSize);
        if ( NULL == handle->pAncDataHostBuffer )
        {
            errCode = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto err_ancillary_buffer;
        }
        handle->ancDataBufferSize = pSettings->ancillaryDataFifoSize;
    }

    /* Success */
    *pHandle = handle;
    deviceHandle->decoders[index] = handle;
    return BERR_SUCCESS;

err_ancillary_buffer:
err_connector_format:
err_caps:
    BAPE_Decoder_Close(handle);
    return errCode;
}

void BAPE_Decoder_Close(
    BAPE_DecoderHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    if ( handle->state != BAPE_DecoderState_eStopped )
    {
        BDBG_WRN(("Implicitly stopping decoder %u on shutdown.", handle->index));
        BAPE_Decoder_Stop(handle);
    }

    /* Disconnect from all mixers, post-processors */
    BAPE_Connector_P_RemoveAllConnections(&handle->node.connectors[BAPE_ConnectorFormat_eStereo]);
    BAPE_Connector_P_RemoveAllConnections(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel]);
    BAPE_Connector_P_RemoveAllConnections(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed]);
    BAPE_Connector_P_RemoveAllConnections(&handle->node.connectors[BAPE_ConnectorFormat_eMono]);

    /* Cleanup */
    if ( handle->pAncDataHostBuffer )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pAncDataHostBuffer);
    }
    if ( handle->pAncDataDspBuffer )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pAncDataDspBuffer);
    }
    if ( handle->pAncDataBufferDescriptor )
    {
        BMEM_Heap_Free(handle->deviceHandle->memHandle, handle->pAncDataBufferDescriptor);
    }
    handle->deviceHandle->decoders[handle->index] = NULL;
    BDBG_OBJECT_DESTROY(handle, BAPE_Decoder);
    BKNI_Free(handle);
}

void BAPE_Decoder_GetDefaultStartSettings(
    BAPE_DecoderStartSettings *pSettings    /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->codec = BAVC_AudioCompressionStd_eMax;
    pSettings->streamType = BAVC_StreamType_eTsMpeg;
    pSettings->ppmCorrection = true;
    pSettings->targetSyncEnabled = true;
    pSettings->maxOutputRate = 48000;
}

static BERR_Code BAPE_Decoder_P_ApplyDsolaSettings(
    BAPE_DecoderHandle handle
    )
{
    BDSP_Raaga_Audio_DsolaConfigParams userConfig;
    BERR_Code errCode;
    unsigned branch,stage;

    if ( handle->startSettings.decodeRateControl && handle->task )
    {
        branch = handle->dsolaBranch;
        stage = handle->dsolaStage;

        errCode = BDSP_Task_GetStageSettings(handle->task, branch, stage, &userConfig, sizeof(userConfig));
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        userConfig.ui32InputPcmFrameSize = (512 * handle->settings.decodeRate)/BAPE_NORMAL_DECODE_RATE;

        errCode = BDSP_Task_SetStageSettings(handle->task, branch, stage, &userConfig, sizeof(userConfig));
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;    
}

static BERR_Code BAPE_Decoder_P_SetupPassthrough(
    BAPE_DecoderHandle handle
    )
{
    BDSP_Raaga_Audio_PassthruConfigParams userConfig;
    BERR_Code errCode;
    unsigned branch,stage,type;

    branch = handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId;
    stage = handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId;

    if ( handle->simul )
    {
        if ( BAPE_P_CodecRequiresGenericPassthru(handle->startSettings.codec) )
        {
            type=BDSP_Raaga_ePassthruType_Simul;
        }
        else
        {
            return BERR_SUCCESS;
        }
    }
    else if ( handle->passthrough )
    {
        if ( handle->startSettings.codec == BAVC_AudioCompressionStd_ePcm )
        {
            type = BDSP_Raaga_ePassthruType_PCM;
        }
        else if ( handle->startSettings.codec == BAVC_AudioCompressionStd_eMlp )
        {
            /* MLP Passthrough is a special algorithm with no configuration */
            return BERR_SUCCESS;
        }
        else
        {
            type = BDSP_Raaga_ePassthruType_SPDIF;
        }
    }
    else
    {
        return BERR_SUCCESS;
    }

    errCode = BDSP_Task_GetStageSettings(handle->task, branch, stage, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.ui32PassthruType = type;
    switch ( handle->startSettings.codec )
    {
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacPlusAdts:
        userConfig.eAacHeaderType = BDSP_Raaga_eAacHeaderType_Adts;
        break;
    case BAVC_AudioCompressionStd_eAacLoas:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        userConfig.eAacHeaderType = BDSP_Raaga_eAacHeaderType_Loas;
        break;
    default:
        break;
    }

    errCode = BDSP_Task_SetStageSettings(handle->task, branch, stage, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ValidateDecodeSettings(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderStartSettings *pSettings,
    BAPE_DecoderStartSettings *pOutputSettings
    )
{
    BDSP_AudioType audioType;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pOutputSettings);
    /* Start by copying the existing settings */
    BKNI_Memcpy(pOutputSettings, pSettings, sizeof(BAPE_DecoderStartSettings));
    /* Check for valid input */
    if ( NULL == pSettings->pContextMap && NULL == pSettings->inputPort )
    {
        BDBG_ERR(("Must specify an input to decode"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    /* Store local copy of the RAVE context map in case it goes out of scope after start. */
    if ( pSettings->pContextMap )
    {
        BKNI_Memcpy(&handle->contextMap, pSettings->pContextMap, sizeof(BAVC_XptContextMap));
        pOutputSettings->pContextMap = &handle->contextMap;
    }
    /* Check for valid STC */
    if ( pSettings->stcIndex > BAPE_CHIP_MAX_STCS )
    {
        BDBG_ERR(("STC Index %u out of range.  Supported values are 0..%u", BAPE_CHIP_MAX_STCS));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    audioType = BAPE_P_GetCodecAudioType(pSettings->codec);
    if ( audioType == BDSP_AudioType_eMax )
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMono].totalConnections > 0 )
    {
        /* Check two things.  One, you can only have mono on certain codecs.  Two, you can only have
           two active PCM paths per node.  */
        if ( !BAPE_P_CodecSupportsMono(pSettings->codec) )
        {
            BDBG_ERR(("Codec %u does not support mono output.  Please remove all mono outputs.", BAPE_P_GetCodecName(pSettings->codec)));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].totalConnections > 0 &&
             handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].totalConnections > 0 )
        {
            BDBG_ERR(("You can only have two types of PCM data from a single decoder.  Please remove outputs from either the stereo, multichannel, or mono connectors."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    if ( handle->passthrough )
    {
        /* Determine if passthrough is valid for this codec */
        if ( !BAPE_P_CodecSupportsPassthrough(pSettings->codec) )
        {
            BDBG_WRN(("Codec %s (%u) does not support compressed passthrough -- Compressed output will be PCM", 
                      BAPE_P_GetCodecName(pSettings->codec), pSettings->codec));
            handle->stereoOnCompressed = true;
            handle->passthrough = false;    /* We're really doing a decode now */
        }
        else
        {
            handle->stereoOnCompressed = false;
        }
    }
    /* Determine if we're trying to decode */
    if ( !handle->passthrough )
    {
        /* Check for License */
        if ( !BDSP_Raaga_IsAudioTypeSupported(audioType) )
        {
            BDBG_ERR(("Codec %s (%u) is not supported on this platform", 
                      BAPE_P_GetCodecName(pSettings->codec), pSettings->codec));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        /* If we're trying simul mode, make sure that's valid */
        if ( handle->simul )
        {
            if ( !BAPE_P_CodecSupportsSimulMode(pSettings->codec) )
            {
                BDBG_WRN(("Codec %s (%u) does not support simultaneous PCM and compressed output - compressed output will be stereo PCM", 
                          BAPE_P_GetCodecName(pSettings->codec), pSettings->codec));
                handle->stereoOnCompressed = true;
            }
            else
            {
                handle->stereoOnCompressed = false;
            }
        }
    }    
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].totalConnections > 0 )
    {
        /* Determine if multichannel is valid */
        if ( BAPE_P_GetCodecMultichannelFormat(pSettings->codec) == BAPE_MultichannelFormat_e2_0 )
        {
            BDBG_WRN(("Codec %s (%u) does not support multichannel output - multichannel output will be stereo", 
                      BAPE_P_GetCodecName(pSettings->codec), pSettings->codec));
            handle->stereoOnMultichannel = true;
        }
        else
        {
            handle->stereoOnMultichannel = false;
        }
    }
    /* Determine if PPM Correction is possible */
    if ( pSettings->decodeRateControl )
    {
        if ( !BDSP_Raaga_IsAudioProcessingSupported(BDSP_AudioProcessing_eDsola) )
        {
            BDBG_WRN(("This platform does not support DSOLA for audio decoder rate control.  Disabling rate control."));
            pOutputSettings->decodeRateControl = false;
        }
        if ( handle->outputStatus.numOutputs[BAPE_DataType_ePcm5_1] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_ePcm7_1] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_ePcmMono] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_ePcmRf] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_eIec61937] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_eIec61937x4] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_eIec61937x16] > 0 )
        {
            BDBG_WRN(("Can not perform audio trick modes with multichannel, compressed, rf, or mono outputs connected.  Diabling rate control."));
            pOutputSettings->decodeRateControl = false;
        }
    }
    if ( pSettings->ppmCorrection )
    {
        BAPE_PathNode *pNode;
        unsigned numFound;
        bool mixedWithDsp;

        BAPE_PathNode_P_FindConsumersBySubtype(&handle->node, BAPE_PathNodeType_eMixer, BAPE_MixerType_eDsp, 1, &numFound, &pNode);
        mixedWithDsp = (numFound > 0) ? true : false;

        if ( handle->outputStatus.numOutputs[BAPE_DataType_ePcm5_1] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_ePcm7_1] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_ePcmMono] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_ePcmRf] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_eIec61937] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_eIec61937x4] > 0 ||
             handle->outputStatus.numOutputs[BAPE_DataType_eIec61937x16] > 0 )
        {
            BDBG_MSG(("PPM Correction is not permitted with multichannel, rf, compressed, or mono outptus.  Disabling PPM correction."));
            pOutputSettings->ppmCorrection = false;
        }
        else if ( pSettings->inputPort )
        {
            BDBG_MSG(("PPM Correction is not permitted with external inputs.  Disabling PPM correction."));
            pOutputSettings->ppmCorrection = false;
        }
        else if ( mixedWithDsp )
        {
            BDBG_MSG(("PPM Correction is not permitted when mixing with the DSP.  Disabling PPM correction."));
            pOutputSettings->ppmCorrection = false;
        }
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_Start(
    BAPE_DecoderHandle handle
    )
{
    BERR_Code errCode;
    unsigned branch, stage, i, stereoBranch, stereoStage;
    const BAPE_DecoderStartSettings *pSettings;
    BAPE_FMT_Descriptor format;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    pSettings = &handle->startSettings;

    BDBG_MSG(("BAPE_Decoder_P_Start(%#x) [index %u]", handle, handle->index));

    /* Do we have compressed outputs directly connected? */
    BDBG_MSG(("%s: %u stereo consumers", handle->node.pName, handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].directConnections));
    BDBG_MSG(("%s: %u multichannel consumers", handle->node.pName, handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].directConnections));
    BDBG_MSG(("%s: %u compressed consumers", handle->node.pName, handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed].directConnections));
    BDBG_MSG(("%s: %u compressed 4x consumers", handle->node.pName, handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed4x].directConnections));
    BDBG_MSG(("%s: %u compressed 16x consumers", handle->node.pName, handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed16x].directConnections));
    BDBG_MSG(("%s: %u mono consumers", handle->node.pName, handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMono].directConnections));

    /* Setup DSP Task First */
    BDSP_Task_GetDefaultCreateSettings(handle->deviceHandle->dspContext, &handle->taskCreateSettings);
    /* Select appropriate dsp */
    handle->taskCreateSettings.dspIndex = handle->dspIndex;
    /* Setup default branch (not malloc'ed - others are) */
    handle->taskCreateSettings.numBranches = 1;
    handle->taskCreateSettings.ppmCorrection = pSettings->ppmCorrection;
    handle->taskCreateSettings.pBranchInfo[0] = &handle->primaryBranch;
    switch ( pSettings->streamType )
    {
    case BAVC_StreamType_eDssEs:
    case BAVC_StreamType_eDssPes:
           handle->taskCreateSettings.timeBaseType = BDSP_AF_P_TimeBaseType_e27Mhz;
           break;
    default:
           handle->taskCreateSettings.timeBaseType = BDSP_AF_P_TimeBaseType_e45Khz;
           break;
    }
    handle->taskCreateSettings.audioTaskDelayMode = (pSettings->delayMode == BAPE_DspDelayMode_eLow)?BDSP_AudioTaskDelayMode_eLow:BDSP_AudioTaskDelayMode_eDefault;
    BAPE_DSP_P_InitBranch(&handle->primaryBranch);
    handle->primaryBranch.ui32NumStages = 1;
    /* Setup Decode/Passthru Stage */
    handle->primaryBranch.sFwStgInfo[0].ui32BranchId = 0;
    handle->primaryBranch.sFwStgInfo[0].ui32StageId = 0;
    handle->primaryBranch.sFwStgInfo[0].eStageType = BDSP_CIT_P_StageType_eDecode;
    handle->primaryBranch.sFwStgInfo[0].uAlgorithm.eDecAudioAlgo = BAPE_P_GetCodecAudioType(handle->startSettings.codec);
    handle->primaryBranch.sFwStgInfo[0].uAudioMode.eDecAudioMode = (handle->passthrough)?BDSP_DecodeMode_ePassThru:BDSP_DecodeMode_eDecode;
    handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.ui32NumSrc = 1; 
    BKNI_Memset(&handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0], 0, 
                sizeof(handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0]));

    if ( pSettings->pContextMap )
    {
        /* Input Hardcoded to RAVE for now (checked above) */
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eRaveBuf;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 2;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eRAVE;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Base;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_End;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Read;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Valid;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[0].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->CDB_Wrap;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32BaseAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Base;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32EndAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_End;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32ReadAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Read;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WriteAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Valid;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.sCircBuffer[1].ui32WrapAddr = BCHP_PHYSICAL_OFFSET + pSettings->pContextMap->ITB_Wrap;  
    }
    else
    {
        BAPE_DfifoGroupCreateSettings dfifoCreateSettings;
        BAPE_DfifoGroupSettings dfifoSettings;

        /* DFIFO Input */
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].eType = BDSP_CIT_P_FwStgSrcDstType_eFMMBuf;
        handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType = BDSP_AF_P_BufferType_eFMM;

        /* Make sure the input is usable.  Don't reference InputPort fields until after this.  */
        errCode = BAPE_InputPort_P_AttachConsumer(pSettings->inputPort, &handle->node, &handle->inputPortFormat);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_attach_input_port;
        }

        /* BDBG_LOG(("Decoder's inputPort format: " BAPE_FMT_P_TO_PRINTF_ARGS(&pSettings->inputPort->format)));            */
        /* BDBG_LOG(("Decoder's inputPort caps: " BAPE_FMT_P_SOURCEMASK_TO_PRINTF_ARGS(&handle->node.inputCapabilities))); */
        /* BDBG_LOG(("Decoder's inputPort caps: " BAPE_FMT_P_TYPEMASK_TO_PRINTF_ARGS(&handle->node.inputCapabilities)));   */

        if ( BAPE_FMT_P_IsLinearPcm(&pSettings->inputPort->format)  &&  pSettings->inputPort->format.sampleRate > 48000) 
        {
            BDBG_ERR(("Decoder doesn't support PCM input sample rate of %u because it exceeds 48000", pSettings->inputPort->format.sampleRate ));
            
            errCode = BERR_TRACE(BERR_NOT_SUPPORTED);
            goto err_dfifo_alloc;
        }

        BAPE_DfifoGroup_P_GetDefaultCreateSettings(&dfifoCreateSettings);
        dfifoCreateSettings.numChannelPairs = BAPE_FMT_P_GetNumChannelPairs(&handle->inputPortFormat);
        errCode = BAPE_DfifoGroup_P_Create(handle->deviceHandle, &dfifoCreateSettings, &handle->inputDfifoGroup);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_dfifo_alloc;
        }
        
        BAPE_DfifoGroup_P_GetSettings(handle->inputDfifoGroup, &dfifoSettings);
        BAPE_InputPort_P_GetFciIds(pSettings->inputPort, &dfifoSettings.input);        
        dfifoSettings.highPriority = (handle->inputPortFormat.sampleRate >= 96000) ? true : false;

        if ( BAPE_FMT_P_IsLinearPcm(&handle->inputPortFormat) )
        {
            handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 2*dfifoCreateSettings.numChannelPairs;
            dfifoSettings.interleaveData = false;
            dfifoSettings.dataWidth = 32;
        }
        else
        {
            handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers = 1;
            dfifoSettings.interleaveData = true;
            dfifoSettings.dataWidth = 16;
        }

        errCode = BAPE_P_AllocateBuffers(handle->deviceHandle, &handle->inputPortFormat, handle->pInputBuffers);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_input_buffers;
        }

        for ( i = 0; i < dfifoCreateSettings.numChannelPairs; i++ )
        {
            uint32_t rbufId;

            /* Setup CIT to have correct ringbuffer ID for each DFIFO -- TODO: There should be a routine in dsp_utils_priv that takes a DFIFO Group handle instead */
            rbufId = BAPE_P_DFIFO_TO_RINBGUFFER(BAPE_DfifoGroup_P_GetHwIndex(handle->inputDfifoGroup, i));
            handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[2*i] = rbufId;
            handle->primaryBranch.sFwStgInfo[0].sStgConnectivity.sSrcDetails[0].uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[(2*i)+1] = rbufId+1;
            dfifoSettings.bufferInfo[2*i].base = handle->pInputBuffers[i]->offset;
            if ( dfifoSettings.interleaveData )
            {
                dfifoSettings.bufferInfo[2*i].length = handle->pInputBuffers[i]->bufferSize;
            }
            else
            {
                unsigned length = handle->pInputBuffers[i]->bufferSize/2;
                dfifoSettings.bufferInfo[2*i].length = dfifoSettings.bufferInfo[(2*i)+1].length = length;
                dfifoSettings.bufferInfo[(2*i)+1].base = handle->pInputBuffers[i]->offset + length;
            }
        }
        errCode = BAPE_DfifoGroup_P_SetSettings(handle->inputDfifoGroup, &dfifoSettings);
        if ( errCode )
        {
            (void)BERR_TRACE(errCode);
            goto err_set_dfifo_settings;
        }
    }

    /* Determine branch/stage outputting data to stereo PCM path  */   
    branch=stage=0;
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].directConnections > 0 )
    {        
        /* Add SRC if required */
        if ( !handle->passthrough && BAPE_P_CodecRequiresSrc(handle->startSettings.codec) && BDSP_Raaga_IsAudioProcessingSupported(BDSP_AudioProcessing_eSrc) )
        {
            errCode = BAPE_DSP_P_AddProcessingStage(&handle->taskCreateSettings, 0, 0, BDSP_AF_P_DistinctOpType_eStereo_PCM, BDSP_AudioProcessing_eSrc, false, &branch, &stage);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_add_decode_pp;
            }
        }
        /* Add DSOLA if required */
        if ( pSettings->decodeRateControl )
        {
            errCode = BAPE_DSP_P_AddProcessingStage(&handle->taskCreateSettings, branch, stage, BDSP_AF_P_DistinctOpType_eStereo_PCM, BDSP_AudioProcessing_eDsola, false, &branch, &stage);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_add_decode_pp;
            }
            handle->dsolaBranch=branch;
            handle->dsolaStage=stage;
        }
        else
        {
            handle->dsolaBranch=BAPE_BRANCH_ID_INVALID;
            handle->dsolaStage=BAPE_STAGE_ID_INVALID;
        }
    }
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].pTaskCreateSettings = &handle->taskCreateSettings;
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].branchId = branch;
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].stageId = stage;
    stereoBranch = branch;
    stereoStage = stage;

    if ( !handle->stereoOnMultichannel )
    {
        /* Determine branch/stage outputting data to multichannel PCM path  */   
        branch=stage=0;
        if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].directConnections > 0 )
        {       
            BDSP_AF_P_DistinctOpType opType = BAPE_DSP_P_GetDataTypeFromConnector(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel]);
            /* Add SRC if required */
            if ( !handle->passthrough && BAPE_P_CodecRequiresSrc(handle->startSettings.codec) && BDSP_Raaga_IsAudioProcessingSupported(BDSP_AudioProcessing_eSrc) )
            {
                errCode = BAPE_DSP_P_AddProcessingStage(&handle->taskCreateSettings, 0, 0, opType, BDSP_AudioProcessing_eSrc, false, &branch, &stage);
                if ( errCode )
                {
                    errCode = BERR_TRACE(errCode);
                    goto err_add_decode_pp;
                }
            }
        }
        handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].pTaskCreateSettings = &handle->taskCreateSettings;
        handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].branchId = branch;
        handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].stageId = stage;
    }
    else
    {
        handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].pTaskCreateSettings = &handle->taskCreateSettings;
        handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].branchId = stereoBranch;
        handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].stageId = stereoStage;
    }

    /* Determine if we need to add passthrough */
    if ( !handle->stereoOnCompressed )
    {
        branch=stage=0;
        if ( handle->simul && BAPE_P_CodecRequiresGenericPassthru(handle->startSettings.codec) )
        {
            /* This should always branch in simul mode */
            errCode = BAPE_DSP_P_AddProcessingStage(&handle->taskCreateSettings, 0, 0, BDSP_AF_P_DistinctOpType_eAuxilaryDataOut, BDSP_AudioProcessing_eGenericPassThru, true, &branch, &stage);
            if ( errCode )
            {
                errCode = BERR_TRACE(errCode);
                goto err_add_passthrough;
            }
        }
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].pTaskCreateSettings = &handle->taskCreateSettings;
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId = branch;
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId = stage;
    }
    else
    {
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].pTaskCreateSettings = &handle->taskCreateSettings;
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId = stereoBranch;
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId = stereoStage;
    }
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x].pTaskCreateSettings = 
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].pTaskCreateSettings;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x].branchId =
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x].stageId =
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x].pTaskCreateSettings = 
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].pTaskCreateSettings;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x].branchId =
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x].stageId =
        handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId;

    /* Prepare all branches to start */
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].directConnections )
    {
        BAPE_Connector_P_GetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], &format);
        format.sampleRate = 0;
        format.ppmCorrection = pSettings->ppmCorrection;
        errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], &format);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_format_change;
        }
    }
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].directConnections )
    {
        BAPE_Connector_P_GetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], &format);
        format.sampleRate = 0;
        if ( handle->stereoOnMultichannel )
        {
            format.type = BAPE_DataType_ePcmStereo;
        }
        else
        {
            switch ( handle->settings.multichannelFormat )
            {
            default:
                /* Should never get here */
            case BAPE_MultichannelFormat_e5_1:
                format.type = BAPE_DataType_ePcm5_1;
                break;
            case BAPE_MultichannelFormat_e7_1:
                format.type = BAPE_DataType_ePcm7_1;
                break;
            }
        }
        errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], &format);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_format_change;
        }
    }
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed].directConnections )
    {
        BAPE_Connector_P_GetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], &format);
        format.sampleRate = 0;
        if ( handle->stereoOnCompressed )
        {
            format.type = BAPE_DataType_ePcmStereo;
        }
        else
        {
            format.type = BAPE_DataType_eIec61937;
            BAPE_FMT_P_SetAudioCompressionStd(&format, handle->startSettings.codec);

            if ( handle->startSettings.codec == BAVC_AudioCompressionStd_eAc3Plus )
            {
                if ( handle->passthrough )
                {
                    format.type = BAPE_DataType_eIec61937x4;    /* AC3+ Passthrough is carried at 4x the rate on HDMI */
                }
                else
                {
                    BAPE_FMT_P_SetAudioCompressionStd(&format, BAVC_AudioCompressionStd_eAc3);  /* While decoding, compressed output will be AC3 not AC3+ */
                }
            }
            else if ( handle->startSettings.codec == BAVC_AudioCompressionStd_eDtshd )
            {
                BAPE_FMT_P_SetAudioCompressionStd(&format, BAVC_AudioCompressionStd_eDts);  /* The core bitstream will be output on the 1x compressed output */
            }
        }
        errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], &format);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_format_change;
        }
    }
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed4x].directConnections )
    {
        BAPE_Connector_P_GetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x], &format);
        format.sampleRate = 0;
        if ( handle->stereoOnCompressed )
        {
            format.type = BAPE_DataType_ePcmStereo;
        }
        else
        {
            format.type = BAPE_DataType_eIec61937x4;
            BAPE_FMT_P_SetAudioCompressionStd(&format, handle->startSettings.codec);
        }
        errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x], &format);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_format_change;
        }
    }
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed16x].directConnections )
    {
        BAPE_Connector_P_GetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x], &format);
        format.sampleRate = 0;
        if ( handle->stereoOnCompressed )
        {
            format.type = BAPE_DataType_ePcmStereo;
        }
        else
        {
            format.type = BAPE_DataType_eIec61937x16;
            BAPE_FMT_P_SetAudioCompressionStd(&format, handle->startSettings.codec);
        }
        errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x], &format);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_format_change;
        }
    }
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMono].directConnections )
    {
        BAPE_Connector_P_GetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eMono], &format);
        format.sampleRate = 0;
        errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x], &format);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_format_change;
        }
    }

    /* set up output samplerate table */
    switch( handle->startSettings.maxOutputRate )
    {
        default:
        case 48000:
            /* use defaults from BDSP */
            handle->taskCreateSettings.pSampleRateMap = NULL;
            break;
        case 96000:
            /* cap decoder output rate at 96k */
            for ( i=0; i<BDSP_AF_P_SampFreq_eMax; i++ )
            {
                switch ( i )
                {
                    default:
                    case BDSP_AF_P_SampFreq_e11_025Khz: 
                    case BDSP_AF_P_SampFreq_e22_05Khz:
                    case BDSP_AF_P_SampFreq_e44_1Khz:
                        handle->sampleRateMap.ui32OpSamplingFrequency[i] = 44100;
                        break;
	                case BDSP_AF_P_SampFreq_e8Khz:	
                    case BDSP_AF_P_SampFreq_e16Khz: 
                    case BDSP_AF_P_SampFreq_e32Khz: 
                        handle->sampleRateMap.ui32OpSamplingFrequency[i] = 32000;
                        break;
                    case BDSP_AF_P_SampFreq_e12Khz:
                    case BDSP_AF_P_SampFreq_e24Khz:
                    case BDSP_AF_P_SampFreq_e48Khz:
                        handle->sampleRateMap.ui32OpSamplingFrequency[i] = 48000;
                        break;
                    case BDSP_AF_P_SampFreq_e64Khz:
                    case BDSP_AF_P_SampFreq_e128Khz:
                        handle->sampleRateMap.ui32OpSamplingFrequency[i] = 64000;
                        break;
                    case BDSP_AF_P_SampFreq_e88_2Khz:
                    case BDSP_AF_P_SampFreq_e176_4Khz:
                        handle->sampleRateMap.ui32OpSamplingFrequency[i] = 88200;
                        break;
                    case BDSP_AF_P_SampFreq_e96Khz:
                    case BDSP_AF_P_SampFreq_e192Khz:	
                        handle->sampleRateMap.ui32OpSamplingFrequency[i] = 96000;
                        break;
                }
            }

            handle->taskCreateSettings.pSampleRateMap = &handle->sampleRateMap;  /* If not set, this will be set to the default rate table in bdsp */
            break;
    }
    
    /* Mono outputs will always start from branch/stage 0 - no SRC or DSOLA. */
    handle->node.connectors[BAPE_ConnectorFormat_eMono].pTaskCreateSettings = &handle->taskCreateSettings;
    handle->node.connectors[BAPE_ConnectorFormat_eMono].branchId = 0;
    handle->node.connectors[BAPE_ConnectorFormat_eMono].stageId = 0;

    errCode = BAPE_PathNode_P_AcquirePathResources(&handle->node);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_build_paths;
    }

    /* Ready */
    handle->sampleRate = 0;
    handle->mode = (unsigned)-1;
    BKNI_Memset(&handle->bitRateInfo, 0, sizeof(handle->bitRateInfo));
    errCode = BDSP_Task_Create(handle->deviceHandle->dspContext, &handle->taskCreateSettings, &handle->task);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_task_create;
    }
    errCode = BAPE_Decoder_SetInterruptHandlers(handle, &handle->interrupts);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_codec_settings;
    }
    errCode = BAPE_Decoder_P_ApplyFramesyncSettings(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_codec_settings;
    }
    errCode = BAPE_Decoder_SetTsmSettings(handle, &handle->tsmSettings);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_codec_settings;
    }

    /* Apply codec settings */
    errCode = BAPE_Decoder_P_ApplyCodecSettings(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_codec_settings;
    }

    /* Configure DSOLA */
    if ( pSettings->decodeRateControl )
    {
        errCode = BAPE_Decoder_P_ApplyDsolaSettings(handle);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_codec_settings;
        }
    }

    /* Configure passthrough if used */
    errCode = BAPE_Decoder_P_SetupPassthrough(handle);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_codec_settings;
    }

    /* Initialize connectors */
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].task = handle->task;
    handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].task = handle->task;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed].task = handle->task;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x].task = handle->task;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x].task = handle->task;
    handle->node.connectors[BAPE_ConnectorFormat_eMono].task = handle->task;

    /* Init ancillary data buffer */
    BAPE_Decoder_P_InitAncillaryDataBuffer(handle);

    /* Link the path resources */
    errCode = BAPE_PathNode_P_ConfigurePathResources(&handle->node);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_config_path_resources;
    }

    #if BDBG_DEBUG_BUILD
        BAPE_Mixer_P_PrintMixers(handle->deviceHandle);
    #endif

    /* Start the consumers */
    errCode = BAPE_PathNode_P_StartPaths(&handle->node);
    if ( errCode )
    {
        goto err_start_path;
    }
    
    #if BDBG_DEBUG_BUILD
        BAPE_Mixer_P_PrintDownstreamNodes(&handle->node);
    #endif
    
#if BAPE_DISABLE_DSP
    #warning Task Start is Disabled!
    BDBG_ERR(("NOT STARTING"));
#else
    errCode = BDSP_Task_Start(handle->task);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_start_task;
    }
#endif

    /* Start the DFIFOs */
    if ( pSettings->inputPort )
    {
        errCode = BAPE_DfifoGroup_P_Start(handle->inputDfifoGroup, false);
        if ( errCode )
        {
            errCode = BERR_TRACE(errCode);
            goto err_dfifo_start;
        }
        BDBG_ASSERT(NULL != pSettings->inputPort->enable);
        pSettings->inputPort->enable(pSettings->inputPort);
    }

    handle->state = BAPE_DecoderState_eStarted;
    return BERR_SUCCESS;

err_dfifo_start:
    BDSP_Task_Stop(handle->task);
err_start_task:
    BAPE_PathNode_P_StopPaths(&handle->node);
err_start_path:
err_config_path_resources:
err_codec_settings:
    BDSP_Task_Destroy(handle->task);
    handle->task = NULL;
err_task_create:
    BAPE_PathNode_P_ReleasePathResources(&handle->node);
err_build_paths:
err_format_change:
err_add_passthrough:
err_add_decode_pp:
    BAPE_Decoder_P_CleanupTaskCreateSetings(handle);    
err_set_dfifo_settings:
    if ( pSettings->inputPort )
    {
        BAPE_P_FreeBuffers(handle->deviceHandle, handle->pInputBuffers);
    }
err_input_buffers:
    if ( pSettings->inputPort )
    {
        BAPE_DfifoGroup_P_Destroy(handle->inputDfifoGroup);
        handle->inputDfifoGroup = NULL;
    }
err_dfifo_alloc:
    if ( pSettings->inputPort )
    {
        BAPE_InputPort_P_DetachConsumer(pSettings->inputPort, &handle->node);
    }
err_attach_input_port:
    return errCode;
}

BERR_Code BAPE_Decoder_Start(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderStartSettings *pSettings
    )
{
    BERR_Code errCode;
    BAPE_PathNode *pNode;
    unsigned numFound;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);

    BDBG_MSG(("BAPE_Decoder_Start(%#x) [index %u]", handle, handle->index));

    if ( NULL == handle->deviceHandle->dspContext )
    {
        BDBG_ERR(("DSP Not avaliable"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( handle->state != BAPE_DecoderState_eStopped )
    {
        BDBG_ERR(("Already running, cannot start"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Get output status */
    BAPE_PathNode_P_GetOutputStatus(&handle->node, &handle->outputStatus);

    /* Validate we either have real-time outputs or don't depending on system requirements */
    BAPE_PathNode_P_FindConsumersBySubtype(&handle->node, BAPE_PathNodeType_eMixer, BAPE_MixerType_eStandard, 1, &numFound, &pNode);
    if ( pSettings->nonRealTime )
    {
        if ( numFound != 0 )
        {
            BDBG_ERR(("No outputs should be connected to a decoder in non-realtime mode."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else
    {
        if ( numFound == 0 )
        {
            BDBG_ERR(("No outputs are connected.  Cannot start."));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
    }

    /* Determine decoder "mode" based on connections first */
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed].directConnections > 0 ||
         handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed4x].directConnections > 0 ||
         handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed16x].directConnections > 0 )
    {
        /* Do we have any stereo or multichannel outputs connected directly? */
        if ( 0 == (handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].directConnections + 
                   handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].directConnections) )
        {
            /* No.  This is a passthrough operation. */
            handle->passthrough = true;
            handle->simul = false;
        }
        else
        {
            /* Yes.  This is "simul", decode + passthrough */
            handle->passthrough = false;
            handle->simul = true;
        }
    }
    else
    {
        /* No compressed outputs.  This is a decode operation. */
        handle->passthrough = false;
        handle->simul = false;
    }

    /* Sanity check settings */
    errCode = BAPE_Decoder_P_ValidateDecodeSettings(handle, pSettings, &handle->startSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pSettings = &handle->startSettings;

    /* Determine multistream linkage */
    errCode = BAPE_Decoder_P_DeriveMultistreamLinkage(handle);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Trigger task and path startup */
    errCode = BAPE_Decoder_P_Start(handle);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Success */
    return BERR_SUCCESS;
}

static void BAPE_Decoder_P_Stop(
    BAPE_DecoderHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    if ( NULL == handle->task )
    {
        BDBG_MSG(("BAPE_Decoder_P_Stop: Decoder %u already stopped.", handle->index));
        return;
    }

    /* Stop DFIFO if needed*/
    if ( handle->startSettings.inputPort )
    {
        BDBG_ASSERT(NULL != handle->startSettings.inputPort->disable);
        handle->startSettings.inputPort->disable(handle->startSettings.inputPort);
        BAPE_DfifoGroup_P_Stop(handle->inputDfifoGroup);
    }

#if BAPE_DISABLE_DSP
    #warning Task Start is Disabled!
    BDBG_ERR(("NOT STOPPING DSP"));
#else
    BDSP_Task_Stop(handle->task);
#endif

    BAPE_PathNode_P_StopPaths(&handle->node);

    if ( handle->startSettings.inputPort  )
    {
        BAPE_P_FreeBuffers(handle->deviceHandle, handle->pInputBuffers);
        BAPE_DfifoGroup_P_Destroy(handle->inputDfifoGroup);
        handle->inputDfifoGroup = NULL;
        BAPE_InputPort_P_DetachConsumer(handle->startSettings.inputPort, &handle->node);
    }

    BDSP_Task_Destroy(handle->task);
    handle->task = NULL;
    BAPE_Decoder_P_CleanupTaskCreateSetings(handle);    
}

void BAPE_Decoder_Stop(
    BAPE_DecoderHandle handle
    )
{
    bool unmute = false;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    BDBG_MSG(("BAPE_Decoder_Stop(%#x) [index %u]", handle, handle->index));

    switch ( handle->state )
    {
    case BAPE_DecoderState_eStopped:
        BDBG_WRN(("Decoder %u Already Stopped.", handle->index));
        return;
    case BAPE_DecoderState_ePaused:
    case BAPE_DecoderState_eDisabledPaused:
        unmute = true;
        break;
    default:
        break;
    }

    /* Stop the task first */
    handle->state = BAPE_DecoderState_eStopped;
    /* Serialize with critical section prior to stopping the task, guarantees isrs are not updating while we stop (they check the state first) */
    BKNI_EnterCriticalSection();
    BKNI_LeaveCriticalSection();

    BAPE_Decoder_P_Stop(handle);

    /* Reset multistream state */
    handle->ddre = NULL;
    handle->fwMixer = NULL;
    handle->fwMixerMaster = true;

    if ( unmute )
    {
        BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], false);
        BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], false);
        BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], false);
        BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eMono], false);
    }
}

BERR_Code BAPE_Decoder_Pause(
    BAPE_DecoderHandle handle
    )
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    BDBG_MSG(("BAPE_Decoder_Pause(%#x) [index %u]", handle, handle->index));

    /* Make sure we're performing a valid state transition */
    switch ( handle->state )
    {
    case BAPE_DecoderState_eStopped:
        BDBG_ERR(("Decoder %u is not started, cannot pause.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BAPE_DecoderState_eStarted:
        break;
    case BAPE_DecoderState_ePaused:
        BDBG_WRN(("Decoder %u already paused.", handle->index));
        return BERR_SUCCESS;
    case BAPE_DecoderState_eDisabled:
    case BAPE_DecoderState_eDisabledPaused:
        BDBG_ERR(("Decoder %u is disabled for flush.  Must complete flush prior to calling Pause.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    default:
        BDBG_ERR(("Unexpected decoder state %u", handle->state));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], true);
    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], true);
    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], true);
    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eMono], true);

    errCode = BDSP_AudioTask_Pause(handle->task);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    handle->state = BAPE_DecoderState_ePaused;
    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_Resume(
    BAPE_DecoderHandle handle
    )
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    BDBG_MSG(("BAPE_Decoder_Resume(%#x) [index %u]", handle, handle->index));

    /* Make sure we're performing a valid state transition */
    switch ( handle->state )
    {
    case BAPE_DecoderState_eStopped:
        BDBG_ERR(("Decoder %u is not started, cannot resume.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BAPE_DecoderState_eStarted:
        BDBG_WRN(("Decoder %u already running.", handle->index));
        return BERR_SUCCESS;
    case BAPE_DecoderState_ePaused:
        break;
    case BAPE_DecoderState_eDisabled:
    case BAPE_DecoderState_eDisabledPaused:
        BDBG_ERR(("Decoder %u is disabled for flush.  Must complete flush prior to calling Resume.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    default:
        BDBG_ERR(("Unexpected decoder state %u", handle->state));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = BDSP_AudioTask_Resume(handle->task);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], false);
    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], false);
    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], false);
    BAPE_Connector_P_SetMute(&handle->node.connectors[BAPE_ConnectorFormat_eMono], false);

    handle->state = BAPE_DecoderState_eStarted;
    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_Advance(
    BAPE_DecoderHandle handle,
    unsigned milliseconds           /* Milliseconds to advance */
    )
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    BDBG_MSG(("BAPE_Decoder_Advance(%#x, %u) [index %u]", handle, milliseconds, handle->index));

    /* Make sure we're performing a valid state transition */
    switch ( handle->state )
    {
    case BAPE_DecoderState_eStopped:
        BDBG_ERR(("Decoder %u is not started, cannot advance.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BAPE_DecoderState_eStarted:
        BDBG_WRN(("Decoder %u running, can't advance.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BAPE_DecoderState_ePaused:
        break;
    case BAPE_DecoderState_eDisabled:
    case BAPE_DecoderState_eDisabledPaused:
        BDBG_ERR(("Decoder %u is disabled for flush.  Must complete flush prior to calling Advance.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    default:
        BDBG_ERR(("Unexpected decoder state %u", handle->state));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = BDSP_AudioTask_Advance(handle->task, milliseconds);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_DisableForFlush(
    BAPE_DecoderHandle handle
    )
{
    BAPE_DecoderState newState = BAPE_DecoderState_eMax;

    BDBG_MSG(("BAPE_Decoder_DisableForFlush(%#x) [index %u]", handle, handle->index));

    /* Make sure we're performing a valid state transition */
    switch ( handle->state )
    {
    case BAPE_DecoderState_eStopped:
        BDBG_ERR(("Decoder %u is not started, cannot disable for flush.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BAPE_DecoderState_eStarted:
        newState = BAPE_DecoderState_eDisabled;
        break;
    case BAPE_DecoderState_ePaused:
        newState = BAPE_DecoderState_eDisabledPaused;
        break;
    case BAPE_DecoderState_eDisabled:
    case BAPE_DecoderState_eDisabledPaused:
        /* No change */
        return BERR_SUCCESS;
    default:
        BDBG_ERR(("Unexpected decoder state %u", handle->state));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* Transition State */
    handle->state = newState;
    BKNI_EnterCriticalSection();
    BKNI_LeaveCriticalSection();
    BAPE_Decoder_P_Stop(handle);

    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_Flush(
    BAPE_DecoderHandle handle
    )
{
    BERR_Code errCode;
    bool paused = false;

    BDBG_MSG(("BAPE_Decoder_Flush(%#x) [index %u]", handle, handle->index));

    /* Make sure we're performing a valid state transition */
    switch ( handle->state )
    {
    case BAPE_DecoderState_eStopped:
        BDBG_ERR(("Decoder %u is not started, cannot flush.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BAPE_DecoderState_eStarted:
    case BAPE_DecoderState_ePaused:
        BDBG_ERR(("Decoder %u is not disabled, cannot flush.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    case BAPE_DecoderState_eDisabled:
        break;
    case BAPE_DecoderState_eDisabledPaused:
        paused = true;
        break;
    default:
        BDBG_ERR(("Unexpected decoder state %u", handle->state));
        BDBG_ASSERT(0);
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = BAPE_Decoder_P_Start(handle);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( paused )
    {
        errCode = BAPE_Decoder_Pause(handle);
        if ( errCode )
        {
            /* Should never happen, but just for completeness */
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

void BAPE_Decoder_GetTsmSettings(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmSettings *pSettings  /* [out] */
    )
{
    BKNI_EnterCriticalSection();
    BAPE_Decoder_GetTsmSettings_isr(handle, pSettings);
    BKNI_LeaveCriticalSection();
}

void BAPE_Decoder_GetTsmSettings_isr(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmSettings *pSettings  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->tsmSettings;
}

BERR_Code BAPE_Decoder_SetTsmSettings(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderTsmSettings *pSettings
    )
{
    BERR_Code errCode;
    BKNI_EnterCriticalSection();
    errCode = BAPE_Decoder_SetTsmSettings_isr(handle, pSettings);
    BKNI_LeaveCriticalSection();
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Set Audio Decoder TSM Settings in isr context
***************************************************************************/
BERR_Code BAPE_Decoder_SetTsmSettings_isr(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderTsmSettings *pSettings
    )
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);

    if ( BAPE_Decoder_P_TaskValid_isr(handle) )
    {
        BDSP_AudioTaskTsmSettings tsmSettings;
        BDSP_AudioTask_GetTsmSettings_isr(handle->task, 0, 0, &tsmSettings);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, ui32STCAddr, BCHP_PHYSICAL_OFFSET + BAPE_CHIP_GET_STC_ADDRESS(handle->startSettings.stcIndex));
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, eTsmEnable, pSettings->tsmEnabled?BDSP_Raaga_eTsmBool_True:BDSP_Raaga_eTsmBool_False);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, eAstmEnable, pSettings->astmEnabled?BDSP_Raaga_eTsmBool_True:BDSP_Raaga_eTsmBool_False);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, ePlayBackOn, pSettings->playback?BDSP_Raaga_eTsmBool_True:BDSP_Raaga_eTsmBool_False);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, ui32AVOffset, pSettings->ptsOffset);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, i32TSMDiscardThreshold, pSettings->thresholds.discard*45);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, i32TSMGrossThreshold, pSettings->thresholds.grossAdjustment*45);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, i32TSMSmoothThreshold, pSettings->thresholds.smoothTrack*45);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, i32TSMSyncLimitThreshold, pSettings->thresholds.syncLimit*45);
        BDBG_MSG(("BAPE_Decoder_SetTsmSettings_isr: eSTCValid = %u", tsmSettings.eSTCValid));
        if ( handle->startSettings.nonRealTime )
        {
            BAPE_DSP_P_SET_VARIABLE(tsmSettings, ui32SwSTCOffset, pSettings->stcOffset);
        }
        errCode = BDSP_AudioTask_SetTsmSettings_isr(handle->task, 0, 0, &tsmSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    handle->tsmSettings = *pSettings;

    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_GetTsmStatus(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmStatus *pStatus  /* [out] */
    )
{
    BERR_Code errCode;
    BKNI_EnterCriticalSection();
    errCode = BAPE_Decoder_GetTsmStatus_isr(handle, pStatus);
    BKNI_LeaveCriticalSection();
    if ( errCode )
    {
        return errCode;     /* BERR_TRACE intentionally omitted */
    }
    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_GetTsmStatus_isr(
    BAPE_DecoderHandle handle,
    BAPE_DecoderTsmStatus *pStatus  /* [out] */
    )
{
    BERR_Code errCode;
    BDSP_AudioTaskTsmStatus tsmStatus;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pStatus);

    if ( BAPE_Decoder_P_TaskValid_isr(handle) )
    {
        errCode = BDSP_AudioTask_GetTsmStatus_isr(handle->task, 0, 0, &tsmStatus);
        if ( errCode )
        {
            BKNI_Memset(pStatus, 0, sizeof(*pStatus));
            pStatus->ptsInfo.ePTSType = BAVC_PTSType_eInterpolatedFromInvalidPTS; /* Mark PTS as invalid */
            return errCode;     /* BERR_TRACE intentionally omitted */
        }
        BAPE_Decoder_P_ConvertTsmStatus_isr(pStatus, &tsmStatus);
    }

    return BERR_SUCCESS;
}

void BAPE_Decoder_GetSettings(
    BAPE_DecoderHandle handle,
    BAPE_DecoderSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);

    *pSettings = handle->settings;
}

BERR_Code BAPE_Decoder_SetSettings(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderSettings *pSettings
    )
{
    BERR_Code errCode = BERR_SUCCESS;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);

    if ( pSettings->decodeRate < (BAPE_NORMAL_DECODE_RATE/2) || pSettings->decodeRate > (2*BAPE_NORMAL_DECODE_RATE) )
    {
        BDBG_ERR(("Audio trick play is supported for 0.5x to 2x playback only. (rate=%u)", pSettings->decodeRate));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( handle->settings.multichannelFormat != pSettings->multichannelFormat )
    {
        if ( handle->task )
        {
            BDBG_ERR(("Cannot change multichannel format while decoder is running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        /* This will be applied later during the next start */
    }

    handle->settings = *pSettings;

    if ( handle->task )
    {
        errCode = BAPE_Decoder_P_ApplyCodecSettings(handle);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        if ( handle->startSettings.decodeRateControl )
        {
            errCode = BAPE_Decoder_P_ApplyDsolaSettings(handle);
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }
        }
    }

    return BERR_SUCCESS;
}

void BAPE_Decoder_GetStatus(
    BAPE_DecoderHandle handle,
    BAPE_DecoderStatus *pStatus     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pStatus);

    BKNI_Memset(pStatus, 0, sizeof(BAPE_DecoderStatus));
    if ( handle->state == BAPE_DecoderState_eStopped )
    {
        pStatus->codec = BAVC_AudioCompressionStd_eMax;
        pStatus->sampleRate = 0;
        pStatus->running = false;
    }
    else
    {
        pStatus->codec = handle->startSettings.codec;
        BAPE_Decoder_GetTsmStatus(handle, &pStatus->tsmStatus);
        pStatus->sampleRate = handle->sampleRate;
        BAPE_Decoder_P_GetCodecStatus(handle, pStatus);
        pStatus->running = true;
        if ( handle->startSettings.inputPort ) {
            pStatus->halted  = handle->startSettings.inputPort->halted;
        }
    }
}

void BAPE_Decoder_GetCodecSettings(
    BAPE_DecoderHandle handle,
    BAVC_AudioCompressionStd codec,
    BAPE_DecoderCodecSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);
    switch ( codec )
    {
    case BAVC_AudioCompressionStd_eAc3:
        *pSettings = handle->ac3Settings;
        break;
    case BAVC_AudioCompressionStd_eAc3Plus:
        *pSettings = handle->ac3PlusSettings;
        break;
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
        *pSettings = handle->aacSettings;
        break;
    case BAVC_AudioCompressionStd_eAacPlusAdts:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        *pSettings = handle->aacPlusSettings;
        break;
    case BAVC_AudioCompressionStd_eDts:
    case BAVC_AudioCompressionStd_eDtshd:
    case BAVC_AudioCompressionStd_eDtsLegacy:
        *pSettings = handle->dtsSettings;
        break;
    case BAVC_AudioCompressionStd_eAdpcm:
        *pSettings = handle->adpcmSettings;
        break;
    default:
        break;
    }
    pSettings->codec = codec;
}

BERR_Code BAPE_Decoder_SetCodecSettings(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderCodecSettings *pSettings
    )
{
    BERR_Code errCode = BERR_SUCCESS;
    bool updateTask=false;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pSettings);
    
    /* Passthrough params are not user-changeable, so no need to set them.
       Default to update task if the codec matches for decode/simul cases. */
    if ( handle->startSettings.codec == pSettings->codec &&
         !handle->passthrough )
    {
        updateTask = true;
    }

    switch ( pSettings->codec )
    {
    case BAVC_AudioCompressionStd_eAc3:
        handle->ac3Settings = *pSettings;
        break;
    case BAVC_AudioCompressionStd_eAc3Plus:
        handle->ac3PlusSettings = *pSettings;
        break;
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
        handle->aacSettings = *pSettings;
        if ( handle->startSettings.codec == BAVC_AudioCompressionStd_eAacLoas ||
             handle->startSettings.codec == BAVC_AudioCompressionStd_eAacAdts )
        {
            /* We don't manage AAC ADTS/LOAS as separate configs */
            updateTask = true;
        }
        break;
    case BAVC_AudioCompressionStd_eAacPlusLoas:
    case BAVC_AudioCompressionStd_eAacPlusAdts:
        handle->aacPlusSettings = *pSettings;
        if ( handle->startSettings.codec == BAVC_AudioCompressionStd_eAacPlusLoas ||
             handle->startSettings.codec == BAVC_AudioCompressionStd_eAacPlusAdts )
        {
            /* We don't manage AAC ADTS/LOAS as separate configs */
            updateTask = true;
        }
        break;
    case BAVC_AudioCompressionStd_eDts:
    case BAVC_AudioCompressionStd_eDtshd:
    case BAVC_AudioCompressionStd_eDtsLegacy:
        handle->dtsSettings = *pSettings;
        if ( handle->startSettings.codec == BAVC_AudioCompressionStd_eDts ||
             handle->startSettings.codec == BAVC_AudioCompressionStd_eDtshd ||
             handle->startSettings.codec == BAVC_AudioCompressionStd_eDtsLegacy )
        {
            /* We don't manage the multitude of DTS variants as separate configs */
            updateTask = true;
        }
        break;
    case BAVC_AudioCompressionStd_eAdpcm:
        handle->adpcmSettings = *pSettings;
        break;
    default:
        updateTask = false;
        break;
    }

    if ( handle->task && updateTask )
    {
        errCode = BAPE_Decoder_P_ApplyCodecSettings(handle);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

void BAPE_Decoder_GetConnector(
    BAPE_DecoderHandle handle,
    BAPE_ConnectorFormat format,
    BAPE_Connector *pConnector /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pConnector);

    switch ( format )
    {
    case BAPE_ConnectorFormat_eStereo:
    case BAPE_ConnectorFormat_eMultichannel:
    case BAPE_ConnectorFormat_eCompressed:
    case BAPE_ConnectorFormat_eCompressed4x:
    case BAPE_ConnectorFormat_eCompressed16x:
    case BAPE_ConnectorFormat_eMono:
        *pConnector = &handle->node.connectors[format];
        break;
    default:
        BDBG_ERR(("Unsupported data path format %u", format));
        *pConnector = NULL;
        break;
    }
}

void BAPE_Decoder_GetInterruptHandlers(
    BAPE_DecoderHandle handle,
    BAPE_DecoderInterruptHandlers *pInterrupts     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pInterrupts);
    *pInterrupts = handle->interrupts;
}

BERR_Code BAPE_Decoder_SetInterruptHandlers(
    BAPE_DecoderHandle handle,
    const BAPE_DecoderInterruptHandlers *pInterrupts
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pInterrupts);
    BKNI_EnterCriticalSection();
    if ( handle->task )
    {
        BDSP_AudioInterruptHandlers interrupts;
        BDSP_AudioTask_GetInterruptHandlers_isr(handle->task, &interrupts);
        interrupts.firstPts.pCallback_isr = BAPE_Decoder_P_FirstPts_isr;
        interrupts.firstPts.pParam1 = handle;
        interrupts.tsmFail.pCallback_isr = BAPE_Decoder_P_TsmFail_isr;
        interrupts.tsmFail.pParam1 = handle;
        interrupts.tsmPass.pCallback_isr = BAPE_Decoder_P_TsmPass_isr;
        interrupts.tsmPass.pParam1 = handle;
        interrupts.sampleRateChange.pCallback_isr = BAPE_Decoder_P_SampleRateChange_isr;
        interrupts.sampleRateChange.pParam1 = handle;
        interrupts.lock.pCallback_isr = pInterrupts->lock.pCallback_isr;
        interrupts.lock.pParam1 = pInterrupts->lock.pParam1;
        interrupts.lock.param2 = pInterrupts->lock.param2;
        interrupts.unlock.pCallback_isr = pInterrupts->unlock.pCallback_isr;
        interrupts.unlock.pParam1 = pInterrupts->unlock.pParam1;
        interrupts.unlock.param2 = pInterrupts->unlock.param2;               
        interrupts.modeChange.pCallback_isr = BAPE_Decoder_P_ModeChange_isr;
        interrupts.modeChange.pParam1 = handle;
        interrupts.bitrateChange.pCallback_isr = BAPE_Decoder_P_BitrateChange_isr;
        interrupts.bitrateChange.pParam1 = handle;
        interrupts.cdbItbOverflow.pCallback_isr = BAPE_Decoder_P_Overflow_isr;
        interrupts.cdbItbOverflow.pParam1 = handle;
        interrupts.statusReady.pCallback_isr = BAPE_Decoder_P_StatusReady_isr;
        interrupts.statusReady.pParam1 = handle;
        interrupts.ancillaryData.pCallback_isr = pInterrupts->ancillaryData.pCallback_isr;
        interrupts.ancillaryData.pParam1 = pInterrupts->ancillaryData.pParam1;
        interrupts.ancillaryData.param2 = pInterrupts->ancillaryData.param2;

        errCode = BDSP_AudioTask_SetInterruptHandlers_isr(handle->task, &interrupts);
        if ( errCode )
        {
            BKNI_LeaveCriticalSection();
            return BERR_TRACE(errCode);
        }
    }
    handle->interrupts = *pInterrupts;
    BKNI_LeaveCriticalSection();
    return BERR_SUCCESS;
}

void BAPE_Decoder_P_SetSampleRate_isr(BAPE_DecoderHandle handle, unsigned sampleRate)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], sampleRate);
    BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], sampleRate);
    BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[BAPE_ConnectorFormat_eMono], sampleRate);
    /* Catch AC3+ Passthrough */
    if ( handle->node.connectors[BAPE_ConnectorFormat_eCompressed].format.type == BAPE_DataType_eIec61937x4 )
    {
        if ( sampleRate == 32000 )
        {
            BDBG_WRN(("AC3 Plus compressed passthrough is not supported at 32kHz sampling rates."));
        }        
        BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], sampleRate*4);
    }
    else
    {
        BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], sampleRate);
    }
    BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed4x], sampleRate*4);
    BAPE_Connector_P_SetSampleRate_isr(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed16x], sampleRate*4);    /* HBR is sent at 4x with 4 samples per clock */
}

void BAPE_Decoder_GetDefaultCdbItbConfig(
    BAPE_DecoderHandle handle,
    BAVC_CdbItbConfig *pConfig  /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(handle);
    BKNI_Memset(pConfig, 0, sizeof(BAVC_CdbItbConfig));
    pConfig->Cdb.Length = 256*1024;
    pConfig->Cdb.Alignment = 6; /* cache line on 4380 */
    pConfig->Itb.Length = 128*1024;
    pConfig->Itb.Alignment = 6; /* cache line on 4380 */
#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE
    pConfig->Cdb.LittleEndian = true;
#else
    pConfig->Cdb.LittleEndian = false;
#endif
}

BERR_Code BAPE_Decoder_SetStcValid_isr(
    BAPE_DecoderHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    if ( BAPE_Decoder_P_TaskValid_isr(handle) )
    {
        BDSP_AudioTaskTsmSettings tsmSettings;
        BERR_Code errCode;
        BDSP_AudioTask_GetTsmSettings_isr(handle->task, 0, 0, &tsmSettings);
        BAPE_DSP_P_SET_VARIABLE(tsmSettings, eSTCValid, BDSP_Raaga_eTsmBool_True);
        errCode = BDSP_AudioTask_SetTsmSettings_isr(handle->task, 0, 0, &tsmSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    return BERR_SUCCESS;
}

static void BAPE_Decoder_P_SetupDefaults(BAPE_DecoderHandle handle)
{
    BDSP_AudioTaskTsmSettings tsmSettings;

    BDSP_AudioTask_GetDefaultTsmSettings(&tsmSettings, sizeof(tsmSettings));
    handle->tsmSettings.tsmEnabled = tsmSettings.eTsmEnable == BDSP_Raaga_eTsmBool_True?true:false;
    handle->tsmSettings.astmEnabled = tsmSettings.eAstmEnable == BDSP_Raaga_eTsmBool_True?true:false;
    handle->tsmSettings.playback = tsmSettings.ePlayBackOn == BDSP_Raaga_eTsmBool_True?true:false;
    handle->tsmSettings.ptsOffset = tsmSettings.ui32AVOffset/45;
    handle->tsmSettings.thresholds.discard = tsmSettings.i32TSMDiscardThreshold/45;
    handle->tsmSettings.thresholds.grossAdjustment = tsmSettings.i32TSMGrossThreshold/45;
    handle->tsmSettings.thresholds.smoothTrack = tsmSettings.i32TSMSmoothThreshold/45;
    handle->tsmSettings.thresholds.syncLimit = tsmSettings.i32TSMSyncLimitThreshold/45;

    BAPE_Decoder_P_GetDefaultCodecSettings(handle);
}

static void BAPE_Decoder_P_ConvertTsmStatus_isr(BAPE_DecoderTsmStatus *pStatus, const BDSP_AudioTaskTsmStatus *pDspStatus)
{
    BKNI_Memset(&pStatus->ptsInfo, 0, sizeof(BAVC_PTSInfo));
    pStatus->ptsInfo.ui32CurrentPTS = pDspStatus->ui32RunningPts;
    switch ( pDspStatus->ePtsType )
    {
    case BDSP_PtsType_eCoded:
        pStatus->ptsInfo.ePTSType = BAVC_PTSType_eCoded;
        break;
    case BDSP_PtsType_eInterpolatedFromValidPTS:
        pStatus->ptsInfo.ePTSType = BAVC_PTSType_eInterpolatedFromValidPTS;
        break;
    default:
        BDBG_WRN(("Invalid DSP PTS type %u", pDspStatus->ePtsType));
        /* Fall through */
    case BDSP_PtsType_eInterpolatedFromInvalidPTS:
        pStatus->ptsInfo.ePTSType = BAVC_PTSType_eInterpolatedFromInvalidPTS;
        break;
    }
    pStatus->ptsStcDifference = pDspStatus->i32PtsToStcPhase;
    pStatus->lastFrameLength = pDspStatus->i32TSMUpperThreshold;
}

static void BAPE_Decoder_P_FirstPts_isr(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus)
{
    BAPE_DecoderHandle handle = pParam1;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);

    if ( handle->interrupts.firstPts.pCallback_isr )
    {
        BAPE_DecoderTsmStatus tsmStatus;
        BAPE_Decoder_P_ConvertTsmStatus_isr(&tsmStatus, pTsmStatus);
        handle->interrupts.firstPts.pCallback_isr(handle->interrupts.firstPts.pParam1, handle->interrupts.firstPts.param2, &tsmStatus);
    }
}

static void BAPE_Decoder_P_TsmFail_isr(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus)
{
    BAPE_DecoderHandle handle = pParam1;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);

    if ( handle->interrupts.tsmFail.pCallback_isr )
    {
        BAPE_DecoderTsmStatus tsmStatus;
        BAPE_Decoder_P_ConvertTsmStatus_isr(&tsmStatus, pTsmStatus);
        handle->interrupts.tsmFail.pCallback_isr(handle->interrupts.tsmFail.pParam1, handle->interrupts.tsmFail.param2, &tsmStatus);
    }
}

static void BAPE_Decoder_P_TsmPass_isr(void *pParam1, int param2, const BDSP_AudioTaskTsmStatus *pTsmStatus)
{
    BAPE_DecoderHandle handle = pParam1;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);

    if ( handle->interrupts.tsmPass.pCallback_isr )
    {
        BAPE_DecoderTsmStatus tsmStatus;
        BAPE_Decoder_P_ConvertTsmStatus_isr(&tsmStatus, pTsmStatus);
        handle->interrupts.tsmPass.pCallback_isr(handle->interrupts.tsmPass.pParam1, handle->interrupts.tsmPass.param2, &tsmStatus);
    }
}

static void BAPE_Decoder_P_SampleRateChange_isr(void *pParam1, int param2, unsigned streamSampleRate, unsigned baseSampleRate)
{
    BAPE_DecoderHandle handle = pParam1;
    unsigned fmmSampleRate=0;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);

    /* Store stream sample rate for user status */
    handle->sampleRate = streamSampleRate;

    /* Determine FMM sample rate */
    if ( handle->passthrough )
    {
        switch ( streamSampleRate )
        {
        case 192000:
        case 176400:
        case 128000:
            fmmSampleRate = streamSampleRate/4;
            break;
        case 96000:
        case 88200:
        case 64000:
            fmmSampleRate = streamSampleRate/2;
            break;
        default:
        case 48000:
        case 44100:
        case 32000:
            fmmSampleRate = streamSampleRate;
            break;
        case 24000:
        case 22050:
        case 16000:
            fmmSampleRate = streamSampleRate*2; /* LSF */
            break;
        case 12000:
        case 11025:
        case  8000:
            fmmSampleRate = streamSampleRate*4; /* QSF */
            break;
        }
    }
    else
    {
        fmmSampleRate = baseSampleRate;
    }

    BDBG_MSG(("Sample Rate Interrupt Received [decoder %u] Stream %u FMM %u", handle->index, streamSampleRate, fmmSampleRate));
    /* Apply sample rate downstream - passthrough is at the stream rate and decode is the base rate */
    BAPE_Decoder_P_SetSampleRate_isr(handle, fmmSampleRate);
    if ( handle->interrupts.sampleRateChange.pCallback_isr )
    {
        /* Send stream rate to the application */
        handle->interrupts.sampleRateChange.pCallback_isr(handle->interrupts.sampleRateChange.pParam1, handle->interrupts.sampleRateChange.param2, streamSampleRate);
    }
}

static void BAPE_Decoder_P_ModeChange_isr(void *pParam1, int param2, unsigned mode)
{
    BAPE_DecoderHandle handle = pParam1;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);
    handle->mode = mode;
    BDBG_MSG(("Mode Change Received [decoder %u] acmod now %u", handle->index, mode));
    if ( handle->interrupts.modeChange.pCallback_isr )
    {
        handle->interrupts.modeChange.pCallback_isr(handle->interrupts.modeChange.pParam1, handle->interrupts.modeChange.param2);
    }
}

static void BAPE_Decoder_P_BitrateChange_isr(void *pParam1, int param2, const BDSP_AudioBitRateChangeInfo *pInfo)
{
    BAPE_DecoderHandle handle = pParam1;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);
    BKNI_Memcpy(&handle->bitRateInfo, pInfo, sizeof(*pInfo));
#if 0
    BDBG_MSG(("Bitrate Interrupt Received [decoder %u]", handle->index));
#endif
    if ( handle->interrupts.bitrateChange.pCallback_isr )
    {
        handle->interrupts.bitrateChange.pCallback_isr(handle->interrupts.bitrateChange.pParam1, handle->interrupts.bitrateChange.param2);
    }
}

static void BAPE_Decoder_P_Overflow_isr(void *pParam1, int param2)
{
    BAPE_DecoderHandle handle = pParam1;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);
    BDBG_MSG(("CDB/ITB Overflow Interrupt Received [decoder %u]", handle->index));
    if ( handle->interrupts.cdbItbOverflow.pCallback_isr )
    {
        handle->interrupts.cdbItbOverflow.pCallback_isr(handle->interrupts.cdbItbOverflow.pParam1, handle->interrupts.cdbItbOverflow.param2);
    }
}

static void BAPE_Decoder_P_StatusReady_isr(void *pParam1, int param2)
{
    BAPE_DecoderHandle handle = pParam1;
    BDSP_AudioInterruptHandlers interrupts;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BSTD_UNUSED(param2);
    BDBG_MSG(("Status Ready Interrupt Received [decoder %u]", handle->index));
    if ( handle->interrupts.statusReady.pCallback_isr )
    {
        handle->interrupts.statusReady.pCallback_isr(handle->interrupts.statusReady.pParam1, handle->interrupts.statusReady.param2);
    }

    /* This interrupt will fire each frame, but we only want the first one.  Disable the interrupt after it fires. */
    BDSP_AudioTask_GetInterruptHandlers_isr(handle->task, &interrupts);
    interrupts.statusReady.pCallback_isr = NULL;
    interrupts.statusReady.pParam1 = handle;
    (void)BDSP_AudioTask_SetInterruptHandlers_isr(handle->task, &interrupts);
}

static BERR_Code BAPE_Decoder_P_InputFormatChange_isr(
    BAPE_PathNode *pNode,
    BAPE_InputPort inputPort
    )
{
    BAPE_DecoderHandle handle;    
    bool needToHalt = false;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(inputPort, BAPE_InputPort);
    BKNI_ASSERT_ISR_CONTEXT();
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    if ( BAPE_FMT_P_IsLinearPcm(&inputPort->format)  &&  inputPort->format.sampleRate > 48000 )
    {
        BDBG_MSG(("Decoder input PCM sample rate of %u exceeds 48000.  Halting capture.", inputPort->format.sampleRate ));
        needToHalt = true;
    }

    /* On the fly format changes are not possible */
    if ( (inputPort->format.type != handle->inputPortFormat.type) )
    {
        BDBG_MSG(("Input data format has changed (%s->%s).  Halting capture.", BAPE_FMT_P_GetTypeName(&handle->inputPortFormat), BAPE_FMT_P_GetTypeName(&inputPort->format)));
        needToHalt = true;
    }

    if ( needToHalt )
    {
        if ( handle->interrupts.inputHalted.pCallback_isr )
        {
           handle->interrupts.inputHalted.pCallback_isr(handle->interrupts.inputHalted.pParam1, handle->interrupts.inputHalted.param2);
        }
        /* Intentionally omitted BERR_TRACE */
        return BERR_NOT_SUPPORTED;
    }
    return BERR_SUCCESS;
}

static void BAPE_Decoder_P_CleanupTaskCreateSetings(BAPE_DecoderHandle handle)
{
    unsigned i;

    /* The first branch is always static.  Others need to be BKNI_Free()'d. */
    /* This is not an ideal programming model, but workable */
    for ( i = 1; i < handle->taskCreateSettings.numBranches; i++ )
    {
        BDBG_ASSERT(NULL != handle->taskCreateSettings.pBranchInfo[i]);
        BKNI_Free(handle->taskCreateSettings.pBranchInfo[i]);
    }
    /* Reset structure to defaults */
    BDSP_Task_GetDefaultCreateSettings(handle->deviceHandle->dspContext, &handle->taskCreateSettings);
}

static BERR_Code BAPE_Decoder_P_ApplyFramesyncSettings(BAPE_DecoderHandle handle)
{
    BDSP_AudioTaskDatasyncSettings datasyncSettings;
    BERR_Code errCode;
    
    errCode = BDSP_AudioTask_GetDatasyncSettings(handle->task, 0, 0, 
                                                 &datasyncSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    datasyncSettings.eEnableTargetSync = handle->startSettings.targetSyncEnabled?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable;
    datasyncSettings.eForceCompleteFirstFrame = handle->startSettings.forceCompleteFirstFrame?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable;
    if ( handle->startSettings.inputPort )
    {
        /* Setup input port specifics */
        datasyncSettings.eEnablePESBasedFrameSync = BDSP_AF_P_eEnable;
        switch ( handle->startSettings.inputPort->type )
        {
        case BAPE_InputPortType_eI2s:            
            BDBG_ASSERT(0 == handle->startSettings.inputPort->index || 1 == handle->startSettings.inputPort->index);
            datasyncSettings.eAudioIpSourceType = (handle->startSettings.inputPort->index == 0) ? BDSP_Raaga_Audio_AudioInputSource_eExtI2s0 : BDSP_Raaga_Audio_AudioInputSource_eCapPortRfI2s;
            datasyncSettings.uAudioIpSourceDetail.ui32SamplingFrequency = handle->startSettings.inputPort->format.sampleRate;
            break;
#if defined BCHP_SPDIF_RCVR_CTRL_STATUS
        case BAPE_InputPortType_eSpdif:
            datasyncSettings.eAudioIpSourceType = BDSP_Raaga_Audio_AudioInputSource_eCapPortSpdif;
            datasyncSettings.uAudioIpSourceDetail.ui32MaiCtrlStatusRegAddr = BCHP_PHYSICAL_OFFSET + BCHP_SPDIF_RCVR_CTRL_STATUS;
            break;
#endif
#if defined BCHP_AUD_FMM_IOP_IN_SPDIF_0_STATUS
        case BAPE_InputPortType_eSpdif:
            datasyncSettings.eAudioIpSourceType = BDSP_Raaga_Audio_AudioInputSource_eCapPortSpdif;
            datasyncSettings.uAudioIpSourceDetail.ui32MaiCtrlStatusRegAddr = BCHP_PHYSICAL_OFFSET + BCHP_AUD_FMM_IOP_IN_SPDIF_0_STATUS;
            break;
#endif
#if defined BCHP_HDMI_RCVR_CTRL_MAI_FORMAT
        case BAPE_InputPortType_eMai:
            datasyncSettings.eAudioIpSourceType = BDSP_Raaga_Audio_AudioInputSource_eCapPortHdmi;
            datasyncSettings.uAudioIpSourceDetail.ui32MaiCtrlStatusRegAddr = BCHP_PHYSICAL_OFFSET + BCHP_HDMI_RCVR_CTRL_MAI_FORMAT;
            break;
#endif
#if defined BCHP_AUD_FMM_IOP_IN_HDMI_0_MAI_FORMAT
        case BAPE_InputPortType_eMai:
            datasyncSettings.eAudioIpSourceType = BDSP_Raaga_Audio_AudioInputSource_eCapPortHdmi;
            datasyncSettings.uAudioIpSourceDetail.ui32MaiCtrlStatusRegAddr = BCHP_PHYSICAL_OFFSET + BCHP_AUD_FMM_IOP_IN_HDMI_0_MAI_FORMAT;
            break;
#endif
        default:
            BDBG_ERR(("Input %s is not supported.", handle->startSettings.inputPort->pName));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }

    /* Codec-Specific Settings (currently only for WMA) */
    switch ( handle->startSettings.codec )
    {
    case BAVC_AudioCompressionStd_eWmaStd:
    case BAVC_AudioCompressionStd_eWmaPro:
        datasyncSettings.uAlgoSpecConfigStruct.sWmaConfig.eWMAIpType = BDSP_Raaga_Audio_WMAIpType_eASF;
        break;
    case BAVC_AudioCompressionStd_eWmaStdTs:
        datasyncSettings.uAlgoSpecConfigStruct.sWmaConfig.eWMAIpType = BDSP_Raaga_Audio_WMAIpType_eTS;
        break;
    case BAVC_AudioCompressionStd_eLpcmDvd:
        datasyncSettings.uAlgoSpecConfigStruct.sLpcmConfig.eLpcmType = BDSP_Raaga_Audio_LpcmAlgoType_eDvd;
        break;
    case BAVC_AudioCompressionStd_eLpcm1394:
        datasyncSettings.uAlgoSpecConfigStruct.sLpcmConfig.eLpcmType = BDSP_Raaga_Audio_LpcmAlgoType_eIeee1394;
        break;
    case BAVC_AudioCompressionStd_eLpcmBd:
        #if 0   /* TODO: Not currently defined in bdsp */
        datasyncSettings.uAlgoSpecConfigStruct.sLpcmConfig.eLpcmType = BDSP_Raaga_Audio_LpcmAlgoType_eBd;
        #else
        datasyncSettings.uAlgoSpecConfigStruct.sLpcmConfig.eLpcmType = BDSP_Raaga_Audio_LpcmAlgoType_eMax-1;
        #endif
        break;
    case BAVC_AudioCompressionStd_eDts:
    case BAVC_AudioCompressionStd_eDtsLegacy:
    case BAVC_AudioCompressionStd_eDtshd:
        /* Set endian of compressed input data */
        datasyncSettings.uAlgoSpecConfigStruct.sDtsConfig.eDtsEndianType = 
            (handle->dtsSettings.codecSettings.dts.littleEndian)?
            BDSP_Raaga_Audio_DtsEndianType_eLITTLE_ENDIAN:BDSP_Raaga_Audio_DtsEndianType_eBIG_ENDIAN;
        break;
    default:
        /* Value doesn't matter */
        break;
    }

    errCode = BDSP_AudioTask_SetDatasyncSettings(handle->task, 0, 0, 
                                                 &datasyncSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

/* Used to synchronize interrupts so they don't try to operate on a stopping task */
static bool BAPE_Decoder_P_TaskValid_isr(BAPE_DecoderHandle handle)
{
#if 0
    switch ( handle->state )
    {
    case BAPE_DecoderState_eStarted:
    case BAPE_DecoderState_ePaused:
        return (handle->task != NULL)?true:false;
    default:
        return false;
    }
#endif
    return (handle->task != NULL)?true:false;
}

static BERR_Code BAPE_Decoder_P_DeriveMultistreamLinkage(BAPE_DecoderHandle handle)
{
    BAPE_PathNode *pNode;
    BAPE_DolbyDigitalReencodeHandle ddre=NULL;
    BAPE_MixerHandle fwMixer=NULL;
    unsigned numFound;
    bool master=true;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    /* Find DDRE and FW Mixer.  Need to determine usage modes based on these. */
    BAPE_PathNode_P_FindConsumersBySubtype(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eDdre, 1, &numFound, &pNode);
    switch ( numFound )
    {
    case 0:
        break;
    case 1:
        ddre = pNode->pHandle;
        break;
    default:
        BDBG_ERR(("Multiple DDRE consumers found downstream from decoder %u.  This is not supported.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    BAPE_PathNode_P_FindConsumersBySubtype(&handle->node, BAPE_PathNodeType_eMixer, BAPE_MixerType_eDsp, 1, &numFound, &pNode);
    switch ( numFound )
    {
    case 0:
        break;
    case 1:
        fwMixer = pNode->pHandle;
        break;
    default:
        BDBG_ERR(("Multiple DSP mixer consumers found downstream from decoder %u.  This is not supported.", handle->index));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if ( fwMixer )
    {
        BAPE_PathConnector *pMaster = fwMixer->master;
        if ( NULL == pMaster )
        {
            BDBG_ERR(("A DSP mixer was found downstream from decoder %u but it does not have a master input designated.  This is not supported.", handle->index));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        pNode = pMaster->pParent;
        if ( pNode == &handle->node )
        {
            /* I am the master directly */
            master = true;
        }
        else
        {
            /* Possibly another post-processing node is between the decoder and FW mixer.  Test if the master between this node and the mixer. */
            master = BAPE_PathNode_P_NodeIsConsumer(&handle->node, pNode);
        }
    }

    /* Success.  Store results */
    handle->ddre = ddre;
    handle->fwMixer = fwMixer;
    handle->fwMixerMaster = master;

    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_EnterUnderflowMode(
    BAPE_DecoderHandle handle
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    if ( handle->state == BAPE_DecoderState_eStarted &&
         handle->startSettings.nonRealTime )
    {
        errCode = BDSP_AudioTask_AudioGapFill(handle->task);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        return BERR_SUCCESS;
    }

    BDBG_ERR(("Decoder must be started in non-realtime mode to enter underflow mode"));
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

