/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/SW7425-5116-12.4/1 $
 * $brcm_Date: 12/6/13 11:57a $
 *
 * Module Description: Audio PI Private Interfaces
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_priv.h $
 * 
 * Hydra_Software_Devel/SW7425-5116-12.4/1   12/6/13 11:57a mpeteri
 * SW7425-5116: back-port changes to 12.4
 * 
 * Hydra_Software_Devel/SW7425-5116/1   12/6/13 11:27a mpeteri
 * SW7425-5116: changes to allow 96k output rate from the decoder
 * 
 * Hydra_Software_Devel/98   10/9/12 11:05p jgarrett
 * SW7425-3831: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-3831/2   10/9/12 5:41a jgarrett
 * SW7425-3831: Merge latest updates
 * 
 * Hydra_Software_Devel/SW7425-3831/1   9/14/12 6:12p jgarrett
 * SW7425-3831: Adding programmable mixer output rate
 * 
 * Hydra_Software_Devel/97   10/9/12 2:30a jgarrett
 * SW7346-914: Adding init flag
 * 
 * Hydra_Software_Devel/96   10/7/12 8:19p jgarrett
 * SW7346-914: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7346-914/4   10/7/12 6:54p jgarrett
 * SW7346-914: Adding toggle bits for ancillary data fifo
 * 
 * Hydra_Software_Devel/SW7346-914/3   10/3/12 3:54a jgarrett
 * SW7346-914: Initializing ancillary data structure prior to start and
 * enabling configuration
 * 
 * Hydra_Software_Devel/SW7346-914/2   9/28/12 12:05p jgarrett
 * SW7346-914: Initial ancillary data implementation
 * 
 * Hydra_Software_Devel/95   9/27/12 10:18a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:49p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/SW7346-914/1   9/27/12 2:52p jgarrett
 * SW7346-914: Initial version of ancillary data parser
 * 
 * Hydra_Software_Devel/94   7/6/12 3:54p jgarrett
 * SW7425-455: Detecting and handling orphan nodes that will not produce
 * output
 * 
 * Hydra_Software_Devel/93   5/16/12 11:45a jgarrett
 * SW7425-3071: Adding programmable heaps
 * 
 * Hydra_Software_Devel/92   5/10/12 6:00p gskerl
 * SW7425-2075: Added prototype for BAPE_InputPort_P_Halt_isr().
 * 
 * Hydra_Software_Devel/91   5/4/12 5:59p gskerl
 * SW7425-2075: Consolidated redundant halted flags from BAPE_MaiInput
 * struct and BAPE_Decoder struct into the BAPE_InputPortObject struct.
 * 
 * Hydra_Software_Devel/90   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/5   5/3/12 3:45p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/89   4/20/12 5:25p jgarrett
 * SW7231-774: Adding DSP mixer watchdog handling
 * 
 * Hydra_Software_Devel/SW7425-2075/4   4/27/12 6:33p gskerl
 * SW7425-2075: Added halted flag to BAPE_Decoder structure.
 * 
 * Hydra_Software_Devel/SW7425-2075/3   4/18/12 2:58p gskerl
 * SW7425-2075: Added definition of BAPE_CHANGE_LOG macro.
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/12/12 1:55p jgarrett
 * SW7425-2075: Revising InputPort format handling
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/88   3/21/12 3:00p gskerl
 * SW7429-18: Added prototype for BAPE_InputPort_P_BurstPreambleToCodec().
 * 
 * Hydra_Software_Devel/87   3/12/12 12:15p gskerl
 * SW7429-18: Removed unreferenced #define statements.
 * 
 * Hydra_Software_Devel/86   3/7/12 4:39p jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/86   3/7/12 4:35p jgarrett
 * SW7435-24: Adding DSP index for decode and dsp mixer
 * 
 * Hydra_Software_Devel/85   2/23/12 10:53a jgarrett
 * SW7405-5003: Removing BKNI_Memcmp where structures aren't copied with
 * BKNI_Memcpy
 * 
 * Hydra_Software_Devel/84   2/21/12 6:41p jgarrett
 * SW7231-363: Propagating codec and mono format changes properly
 * 
 * Hydra_Software_Devel/83   2/16/12 3:00p jgarrett
 * SW7552-188: Removing dummy mixer outputs.  MAI CBIT formatting no
 * longer requires this
 * 
 * Hydra_Software_Devel/82   2/15/12 2:39p gskerl
 * SW7429-18: Added audio format fields to the BAPE_InputCapture struct.
 * 
 * Hydra_Software_Devel/81   2/9/12 11:21a jgarrett
 * SW7425-2074: Enabling input halt and revising MAI shutown for
 * multichannel PCM
 * 
 * Hydra_Software_Devel/80   2/8/12 4:48p jgarrett
 * SW7346-572: Adding playback settings
 * 
 * Hydra_Software_Devel/79   2/3/12 4:29p jgarrett
 * SW7425-2268: Initial bringup of voice conferencing support
 * 
 * Hydra_Software_Devel/78   2/2/12 7:04p gskerl
 * SW7429-18: Removed prototypes for all SpdifRx_P_* functions because
 * their functionality has been moved into bape_spdif_input.c and
 * bape_mai_input.c.
 * 
 * Hydra_Software_Devel/77   2/2/12 4:48p jgarrett
 * SW7425-2268: Adding EchoCanceller
 * 
 * Hydra_Software_Devel/76   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/75   1/27/12 5:11p jgarrett
 * SW7429-55: Changing ncoRequired to builtinNco
 * 
 * Hydra_Software_Devel/74   1/27/12 4:49p jgarrett
 * SW7429-55: Adding ncoRequired field to OutputPort
 * 
 * Hydra_Software_Devel/73   1/17/12 2:50p gskerl
 * SW7429-18: Changed argument to BAPE_SpdifRx_P_GetRxStatus() from
 * BAPE_MaiInputFormatDetectionStatus to
 * BAPE_SpdifInputFormatDetectionStatus.
 * 
 * Hydra_Software_Devel/72   1/6/12 5:27p gskerl
 * SW7429-18: Added BAPE_P_NUM_ELEMS macro
 * 
 * Hydra_Software_Devel/71   12/7/11 4:50p jgarrett
 * SW7429-18: Extending input port FCI ID's to 16 bits
 * 
 * Hydra_Software_Devel/70   12/2/11 3:19p jgarrett
 * SW7346-578: Adding ability to change some mixer settings without
 * destroying and re-creating the mixer
 * 
 * Hydra_Software_Devel/69   12/1/11 6:45p gskerl
 * SW7429-18: Don't define the srcCoeffAllocated[] field of BAPE_Device
 * when BAPE_CHIP_MAX_SRC_COEFF_CHUNKS isn't defined.
 * 
 * Hydra_Software_Devel/68   11/30/11 4:26p jgarrett
 * SW7429-18: Adding stubs for SPDIF/MAI inputs on 7429
 * 
 * Hydra_Software_Devel/67   11/14/11 3:23p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/4   11/4/11 5:02p gskerl
 * SW7429-18: Merged in changes from /main/Hydra_Software_Devel.
 * 
 * Hydra_Software_Devel/SW7429-18/3   11/4/11 4:41p jgarrett
 * SW7429-18: Adding channel status bits
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/26/11 12:44p jgarrett
 * SW7429-18: Merging latest changes from main branch
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/66   11/2/11 12:40p gskerl
 * SW7231-129: Added sampleRateConverterRampStep to BAPE_Device structure.
 * 
 * Hydra_Software_Devel/65   11/1/11 2:12p gskerl
 * SW7231-129: Added outputVolumeRampState to BAPE_Device structure.
 * 
 * Hydra_Software_Devel/64   10/25/11 1:17p gskerl
 * SW7231-129: Added timebase field to BAPE_AudioNco struct.
 * 
 * Hydra_Software_Devel/63   10/14/11 12:11p jgarrett
 * SW7425-468: Adding hooks for NRT DSP mixer support
 * 
 * Hydra_Software_Devel/62   9/19/11 3:21p gskerl
 * SW7231-129: Added prototype for BAPE_P_ReleaseUnusedPathResources().
 * 
 * Hydra_Software_Devel/61   9/14/11 6:58p jgarrett
 * SW7425-1045: Adding unmapped settings and status values for some
 * decoder algorithms
 * 
 * Hydra_Software_Devel/60   8/26/11 9:50p sgadara
 * SWDTV-6627: [35233] Add Equalizer support
 * 
 * Hydra_Software_Devel/59   8/25/11 6:22p gskerl
 * SW7425-1119: Changed return type for BAPE_MixerInterface.stop to be
 * void
 * 
 * Hydra_Software_Devel/58   8/24/11 1:44p jgarrett
 * SWDTV-8271: Reversing SPDIF/HDMI input ISRs on 352xx
 * 
 * Hydra_Software_Devel/57   8/24/11 12:47p jgarrett
 * SWDTV-8271: Fixing #includes for SPDIF_RCVR_1
 * 
 * Hydra_Software_Devel/56   8/24/11 11:53a jgarrett
 * SW7425-724: Adding RF Audio Encoder
 * 
 * Hydra_Software_Devel/55   8/23/11 1:25p gskerl
 * SW7425-1119: Added prototypes and stubs for BAPE_Mixer_Start() and
 * BAPE_Mixer_Stop(). For now, they just return BERR_NOT_SUPPORTED
 * 
 * Hydra_Software_Devel/52   8/2/11 3:27p jgarrett
 * SWDTV-8150: Adding mixer output connection support
 * 
 * Hydra_Software_Devel/51   7/15/11 5:51p jgarrett
 * SWDTV-6761: Adding StudioSound API
 * 
 * Hydra_Software_Devel/50   7/8/11 6:38p jgarrett
 * SWDTV-6760: Adding I2sMultiOutput
 * 
 * Hydra_Software_Devel/49   7/8/11 4:24p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/48   7/6/11 4:55p jgarrett
 * SW7358-62: Storing a local copy of the XptContextMap structure
 * 
 * Hydra_Software_Devel/47   6/29/11 5:00p jgarrett
 * SW7425-620: Allowing multichannel/compressed outputs from a decoder to
 * override to PCM if the codec does not support multichannel/compressed
 * output
 * 
 * Hydra_Software_Devel/46   6/22/11 12:26p gskerl
 * SW7425-321: Added data structures for NCOs (cloned from PLLs)
 * 
 * Hydra_Software_Devel/45   6/20/11 7:02p jgarrett
 * SW7425-406: Adding input scaling coefficients to mixer
 * 
 * Hydra_Software_Devel/44   6/16/11 3:06p gskerl
 * SW7425-321: Renamed BAPE_PllStatus to BAPE_AudioPll, pllStatus to
 * audioPll, eliminated unused baseAddress and fsChannel fields, added
 * populating of baseSampleRate and freqCh1 fields.
 * 
 * Hydra_Software_Devel/43   6/14/11 7:18p gskerl
 * SW7425-321: Added prototype for BAPE_Mixer_P_PrintMixers()
 * 
 * Hydra_Software_Devel/42   6/14/11 3:45p jgarrett
 * SW7425-406: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-406/2   6/13/11 7:27p jgarrett
 * SW7425-406: Updates for new BDSP_Task_AddStageInput APIs
 * 
 * Hydra_Software_Devel/SW7425-406/1   6/3/11 6:28p jgarrett
 * SW7425-406: Adding inter-task buffer descriptors to connection
 * 
 * Hydra_Software_Devel/41   6/1/11 6:30p jgarrett
 * SW7425-406: Adding DSP Mixer connection resources
 * 
 * Hydra_Software_Devel/40   5/31/11 6:37p jgarrett
 * SW7425-406: Adding initial DSP mixer code
 * 
 * Hydra_Software_Devel/39   5/27/11 7:49p gskerl
 * SW7425-321: Split mixer code into multiple files. Added framework for
 * dispatching mixer API calls to multiple mixer types.
 * 
 * Hydra_Software_Devel/38   5/27/11 4:56p jgarrett
 * SW7425-408: Propagating decoder settings for AAC/AC3 to DDRE in MS11
 * usage modes
 * 
 * Hydra_Software_Devel/37   5/20/11 5:16p jgarrett
 * SW7425-402: Adding error code to handle output enable callback failing.
 * 
 * Hydra_Software_Devel/36   5/18/11 2:29p gskerl
 * SW7231-128: Added BCHP_PWR support to APE
 * 
 * Hydra_Software_Devel/35   5/14/11 11:56a jgarrett
 * SW7425-407: Adding DV258 Support
 * 
 * Hydra_Software_Devel/34   5/14/11 11:33a jgarrett
 * SWDTV-6763: Adding 3D Surround
 * 
 * Hydra_Software_Devel/33   5/13/11 6:39p jgarrett
 * SW7425-410: Adding AVL
 * 
 * Hydra_Software_Devel/32   5/12/11 4:39p piyushg
 * SW7425-401: Added PCM playback support for non-interleaved and
 * multichannel data
 * 
 * Hydra_Software_Devel/31   5/12/11 12:01p gskerl
 * SW7422-354: Fixed to support APE compilation on 35230
 * 
 * Hydra_Software_Devel/30   5/11/11 7:04p jgarrett
 * SW7422-410: Adding CustomProcessing
 * 
 * Hydra_Software_Devel/29   5/2/11 7:37p gskerl
 * SW7422-354: Added enum for BAPE_OutputPortType
 * 
 * Hydra_Software_Devel/28   4/20/11 7:02p gskerl
 * SW7425-384: Refactored BAPE_P_SetFsTiming_isr() to improve PLLCLKSEL
 * logic and to add support for multiple DACS
 * 
 * Hydra_Software_Devel/27   4/18/11 10:09p jgarrett
 * SW7425-361: Refactoring DSP branch decisions
 * 
 * Hydra_Software_Devel/26   4/17/11 1:55p jgarrett
 * SW7425-288: Adding audio codec to metadata
 * 
 * Hydra_Software_Devel/25   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/24   4/11/11 5:54p jgarrett
 * SWDTV-6305: Adding ADC/RF Inputs for DTV
 * 
 * Hydra_Software_Devel/23   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/22   3/27/11 12:32p piyushg
 * SW7422-364: Rearrange Callback enable/disable calls and other
 * bug fixes for HDMI input.
 * 
 * Hydra_Software_Devel/21   3/24/11 10:28a jgarrett
 * SW7422-364: Fixing compilation errors on systems without HDMI or SPDIF
 * input
 * 
 * Hydra_Software_Devel/20   3/23/11 11:07a piyushg
 * SW7422-364: Initial checking for HDMI input support.
 * 
 * Hydra_Software_Devel/19   3/22/11 3:01p gskerl
 * SW7422-146: Changed audio output connector callbacks to take the output
 * connector as an argument
 * 
 * Hydra_Software_Devel/18   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/17   3/18/11 6:04p gskerl
 * SW7422-146: Changed audio input connector callbacks to take the connect
 * handle as an argument
 * 
 * Hydra_Software_Devel/16   3/11/11 6:02p jgarrett
 * SW7422-146: Decoder supports external inputs
 * 
 * Hydra_Software_Devel/15   3/10/11 7:03p jgarrett
 * SW7422-146: Refactored DFIFO code, added support for input capture from
 * compressed/multichannel
 * 
 * Hydra_Software_Devel/14   3/9/11 4:42p piyushg
 * SW7422-146: Add HDMI and SPDIF callback handles to BAPE_Device
 * structure.
 * 
 * Hydra_Software_Devel/13   3/3/11 6:31p jgarrett
 * SW7422-146: Adding SRC and DSOLA and path review feedback
 * 
 * Hydra_Software_Devel/12   3/1/11 3:26p jgarrett
 * SW7422-146: Moving samlpe rate to input port from input capture
 * 
 * Hydra_Software_Devel/11   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/10   2/22/11 5:43p jgarrett
 * SW7422-146: Implemented type renaming based on filter graph review
 * comments
 * 
 * Hydra_Software_Devel/9   2/15/11 4:08p jgarrett
 * SW7422-146: Adding additional codec settings and types
 * 
 * Hydra_Software_Devel/8   2/10/11 5:45p gskerl
 * SW7422-146: Changed calling sequence for the audio output setMclk_isr
 * callbacks
 * 
 * Hydra_Software_Devel/7   2/2/11 2:17p jgarrett
 * SW7422-146: Adding decoder status placeholder
 * 
 * Hydra_Software_Devel/6   1/28/11 3:38p jgarrett
 * SW7422-146: Adding support for simul handling of DDP/WMA-Pro/Generic
 * 
 * Hydra_Software_Devel/5   1/27/11 5:47p jgarrett
 * SW7422-146: Adding support for multichannel and ac3 user parameters
 * 
 * Hydra_Software_Devel/4   1/19/11 2:58p jgarrett
 * SW7422-146: Initial decode/passthrough of ac3
 * 
 * Hydra_Software_Devel/3   1/7/11 5:21p jgarrett
 * SW7422-146: Adding SPDIF/HDMI input APIs
 * 
 * Hydra_Software_Devel/2   1/6/11 2:33p jgarrett
 * SW7422-146: Adding initial input capture API
 * 
 * Hydra_Software_Devel/1   12/17/10 4:11p jgarrett
 * SW7422-146: Nexus APE integration on 7422
 * 
 ***************************************************************************/

#ifndef BAPE_PRIV_H_
#define BAPE_PRIV_H_

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bdsp.h"
#include "blst_slist.h"
#include "blst_squeue.h"
#include "bape_chip_priv.h"
#include "bape_reg_priv.h"
#if defined BCHP_HDMI_RCVR_CTRL_REG_START
#include "bchp_hdmi_rcvr_ctrl.h"
#elif defined BCHP_AUD_FMM_IOP_IN_HDMI_0_REG_START
#include "bchp_aud_fmm_iop_in_hdmi_0.h"
#endif
#if defined BCHP_SPDIF_RCVR_CTRL_REG_START
#include "bchp_spdif_rcvr_ctrl.h"
#include "bchp_spdif_rcvr_esr.h"
#include "bchp_hdmi_rcvr_ctrl.h"
#include "bchp_hdmi_rcvr_esr.h"
#elif defined BCHP_SPDIF_RCVR_CTRL0_REG_START
#include "bchp_spdif_rcvr_ctrl0.h"
#include "bchp_spdif_rcvr_ctrl1.h"
#include "bchp_spdif_rcvr_esr0.h"
#include "bchp_spdif_rcvr_esr1.h"
#elif defined BCHP_AUD_FMM_IOP_IN_SPDIF_0_REG_START
#include "bchp_aud_fmm_iop_in_spdif_0.h"
#endif

/* Debug objects */
BDBG_OBJECT_ID_DECLARE(BAPE_Device);
BDBG_OBJECT_ID_DECLARE(BAPE_BufferNode);
BDBG_OBJECT_ID_DECLARE(BAPE_Decoder);
BDBG_OBJECT_ID_DECLARE(BAPE_Mixer);
BDBG_OBJECT_ID_DECLARE(BAPE_OutputPort);
BDBG_OBJECT_ID_DECLARE(BAPE_PathConnector);
BDBG_OBJECT_ID_DECLARE(BAPE_PathConnection);
BDBG_OBJECT_ID_DECLARE(BAPE_InputPort);
BDBG_OBJECT_ID_DECLARE(BAPE_PathNode);
BDBG_OBJECT_ID_DECLARE(BAPE_Playback);
BDBG_OBJECT_ID_DECLARE(BAPE_InputCapture);
BDBG_OBJECT_ID_DECLARE(BAPE_EchoCanceller);

/* Shorthand to indicate the number of elements in an array. */
#define BAPE_P_NUM_ELEMS(x)  (sizeof(x) / sizeof (x)[0])

/***************************************************************************
Summary:
A macro to compare two values and print a BDBG message that contains one 
value if they match, or both values if they differ. 
 
For example, this code: 
    BAPE_LOG_CHANGE(BDBG_LOG, "  Sample Rate     ", "%u",
                        oldSampleRate, oldSampleRate, 
                        newSampleRate, newSampleRate );
 
Will print this if both oldSampleRate and newSampleRate are 48000.
    00:00:01.003 bape_fmt_priv:   Sample Rate     : 48000
 
And will print this if oldSampleRate is 48000 and newSampleRate is 768000: 
    00:00:01.003 bape_fmt_priv:   Sample Rate     : 48000 -> 768000
 
***************************************************************************/
#define BAPE_LOG_CHANGE(bdbg_log_macro, heading, pfmt, oldval, oldprint, newval, newprint)           \
        (  (oldval) != (newval)   	 											                     \
            ? 	   bdbg_log_macro(("%s: " pfmt " -> " pfmt , heading,	 (oldprint), (newprint )))   \
            : 	   bdbg_log_macro(("%s: " pfmt, heading, (newprint) ))			        	         \
        )

/***************************************************************************
Summary:
PLL Status
***************************************************************************/
typedef struct BAPE_AudioPll
{
    unsigned baseSampleRate;
    unsigned long freqCh1;
    BLST_S_HEAD(PllMixerList, BAPE_Mixer) mixerList;    /* List of mixers using this PLL */
    BAPE_PllSettings settings;
} BAPE_AudioPll;

/***************************************************************************
Summary:
PLL Status
***************************************************************************/
typedef struct BAPE_AudioNco
{
    unsigned baseSampleRate;
    unsigned long ncoFreq;
    BAVC_Timebase timebase;
    BLST_S_HEAD(NcoMixerList, BAPE_Mixer) mixerList;    /* List of mixers using this NCO */
} BAPE_AudioNco;

/***************************************************************************
Summary:
L3 interrupt structure
***************************************************************************/
typedef struct BAPE_L3Interrupt
{
    BINT_CallbackFunc callback_isr;
    void *pParam1;
    int param2;
} BAPE_L3Interrupt;

/***************************************************************************
Summary:
Invalid Branch/Stage ID's
***************************************************************************/
#define BAPE_BRANCH_ID_INVALID ((unsigned)-1)
#define BAPE_STAGE_ID_INVALID ((unsigned)-1)

/***************************************************************************
Summary:
FCI ID Type
***************************************************************************/
typedef uint16_t BAPE_FciId;

/***************************************************************************
Summary:
Invalid FCI ID
***************************************************************************/
#define BAPE_FCI_ID_INVALID (0x3ff)

/***************************************************************************
Summary:
FCI ID Group
***************************************************************************/
typedef struct BAPE_FciIdGroup
{
    BAPE_FciId ids[BAPE_ChannelPair_eMax];
} BAPE_FciIdGroup;

/***************************************************************************
Summary:
Init FCI ID Group
***************************************************************************/
#define BAPE_FciIdGroup_Init(pGroup) \
do \
{ \
    unsigned i; \
    for ( i = 0; i < BAPE_ChannelPair_eMax; i++ ) \
    { \
        (pGroup)->ids[i] = BAPE_FCI_ID_INVALID; \
    } \
} while (0)

/***************************************************************************
Summary:
Determine number of active channel pairs in an FCI group
***************************************************************************/
#define BAPE_FciIdGroup_GetNumChannelPairs(group) (((group).ids[3] != BAPE_FCI_ID_INVALID)?4:((group).ids[2] != BAPE_FCI_ID_INVALID)?3:((group).ids[1] != BAPE_FCI_ID_INVALID)?2:((group).ids[0] != BAPE_FCI_ID_INVALID)?1:0)

/***************************************************************************
Summary:
Determine number of active channel pairs in an FCI group
***************************************************************************/
#define BAPE_FciIdGroup_IsEqual(group1,group2) (((group1).ids[0] == (group2).ids[0]) && ((group1).ids[1] == (group2).ids[1]) && ((group1).ids[2] == (group2).ids[2]) && ((group1).ids[3] == (group2).ids[3]))

/***************************************************************************
Summary:
Source FIFO Group Handle
***************************************************************************/
typedef struct BAPE_SfifoGroup *BAPE_SfifoGroupHandle;

/***************************************************************************
Summary:
Destination FIFO Group Handle
***************************************************************************/
typedef struct BAPE_DfifoGroup *BAPE_DfifoGroupHandle;

/***************************************************************************
Summary:
Sample Rate Converter Group Handle
***************************************************************************/
typedef struct BAPE_SrcGroup *BAPE_SrcGroupHandle;

/***************************************************************************
Summary:
Sample Rate Converter Group Handle
***************************************************************************/
typedef struct BAPE_SrcMemory *BAPE_SrcMemoryHandle;

/***************************************************************************
Summary:
Mixer Group Handle
***************************************************************************/
typedef struct BAPE_MixerGroup *BAPE_MixerGroupHandle;

/***************************************************************************
Summary:
Loopback Group Handle
***************************************************************************/
typedef struct BAPE_LoopbackGroup *BAPE_LoopbackGroupHandle;

/***************************************************************************
Summary:
Dummysink Group Handle
***************************************************************************/
typedef struct BAPE_DummysinkGroup *BAPE_DummysinkGroupHandle;

/***************************************************************************
Summary:
Decoder State
***************************************************************************/
typedef enum BAPE_DecoderState
{
    BAPE_DecoderState_eStopped,             /* Not running */
    BAPE_DecoderState_eStarted,             /* Running in normal operation */
    BAPE_DecoderState_ePaused,              /* Running, paused */
    BAPE_DecoderState_eDisabled,            /* Prepared for flush from started state */
    BAPE_DecoderState_eDisabledPaused,      /* Prepared for flush from paused state */
    BAPE_DecoderState_eMax
} BAPE_DecoderState;

/***************************************************************************
Summary:
Buffer Node
***************************************************************************/
typedef struct BAPE_BufferNode
{
    BDBG_OBJECT(BAPE_BufferNode)
    BLST_S_ENTRY(BAPE_BufferNode) node;
    void *pMemory;
    uint32_t offset;
    unsigned bufferSize;
    bool allocated;
    uint8_t poolIndex;
} BAPE_BufferNode;

#include "bape_fmt_priv.h"

/***************************************************************************
Summary:
Device Handle
***************************************************************************/
#define BAPE_MAX_BUFFER_POOLS (4)
typedef struct BAPE_Device
{
    BDBG_OBJECT(BAPE_Device)

    /* Open Parameters */
    BCHP_Handle chpHandle;
    BREG_Handle regHandle;
    BMEM_Handle memHandle;
    BINT_Handle intHandle;
    BTMR_Handle tmrHandle;
    BDSP_Handle dspHandle;
    BAPE_Settings settings;
    
    /* Ramp Step Sizes */
    uint32_t outputVolumeRampStep;
    uint32_t sampleRateConverterRampStep;

    /* Buffer Lists */
    struct
    {
        BLST_S_HEAD(FreeBufferList, BAPE_BufferNode) freeList;        
        BLST_S_HEAD(AllocatedBufferList, BAPE_BufferNode) allocatedList;        
        BAPE_FMT_Capabilities capabilities;
        unsigned bufferSize, numFreeBuffers;
    } buffers[BAPE_MAX_BUFFER_POOLS];

    /* Software resource allocation */
#if BAPE_CHIP_MAX_DECODERS > 0
    BDSP_ContextHandle       dspContext;
    unsigned                 numDsps;
    BAPE_DecoderHandle       decoders[BAPE_CHIP_MAX_DECODERS];
    struct 
    {
        BAPE_DecoderState state;
        BAPE_DecoderStartSettings startSettings;
    } decoderWatchdogInfo[BAPE_CHIP_MAX_DECODERS];
#endif

#if BAPE_CHIP_MAX_INPUT_CAPTURES > 0
    BAPE_InputCaptureHandle  inputCaptures[BAPE_CHIP_MAX_INPUT_CAPTURES];
    struct
    {
        bool restartRequired;
        BAPE_InputCaptureStartSettings startSettings;
    } inputCaptureWatchdogInfo[BAPE_CHIP_MAX_INPUT_CAPTURES];
#endif
#if BAPE_CHIP_MAX_PLAYBACKS > 0
    BAPE_PlaybackHandle      playbacks[BAPE_CHIP_MAX_PLAYBACKS];
    struct
    {
        bool restartRequired;
        BAPE_PlaybackStartSettings startSettings;
    } playbackWatchdogInfo[BAPE_CHIP_MAX_PLAYBACKS];
#endif
#if BAPE_CHIP_MAX_DACS > 0
    BAPE_DacHandle           dacs[BAPE_CHIP_MAX_DACS];
#endif
#if BAPE_CHIP_MAX_I2S_OUTPUTS > 0
    BAPE_I2sOutputHandle     i2sOutputs[BAPE_CHIP_MAX_I2S_OUTPUTS];
#endif
#if BAPE_CHIP_MAX_I2S_MULTI_OUTPUTS > 0
    BAPE_I2sMultiOutputHandle     i2sMultiOutputs[BAPE_CHIP_MAX_I2S_MULTI_OUTPUTS];
#endif
#if BAPE_CHIP_MAX_SPDIF_OUTPUTS > 0
    BAPE_SpdifOutputHandle   spdifOutputs[BAPE_CHIP_MAX_SPDIF_OUTPUTS];
#endif
#if BAPE_CHIP_MAX_MAI_OUTPUTS > 0
    BAPE_MaiOutputHandle     maiOutputs[BAPE_CHIP_MAX_MAI_OUTPUTS];
#endif
#if BAPE_CHIP_MAX_OUTPUT_CAPTURES > 0
    BAPE_OutputCaptureHandle outputCaptures[BAPE_CHIP_MAX_OUTPUT_CAPTURES];
#endif
#if BAPE_CHIP_MAX_DUMMYSINKS > 0
    BAPE_DummyOutputHandle     dummyOutputs[BAPE_CHIP_MAX_DUMMYSINKS];
#endif
#if BAPE_CHIP_MAX_I2S_INPUTS > 0
    BAPE_I2sInputHandle      i2sInputs[BAPE_CHIP_MAX_I2S_INPUTS];
#endif
#if BAPE_CHIP_MAX_SPDIF_INPUTS > 0
    BAPE_SpdifInputHandle    spdifInputs[BAPE_CHIP_MAX_SPDIF_INPUTS];
#endif
#if BAPE_CHIP_MAX_MAI_INPUTS > 0
    BAPE_MaiInputHandle      maiInputs[BAPE_CHIP_MAX_MAI_INPUTS];
#endif
#if BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS > 0
    BAPE_AudioReturnChannelHandle      audioReturnChannels[BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS];
#endif
#if BAPE_CHIP_MAX_RFMODS > 0
    BAPE_RfModHandle         rfmods[BAPE_CHIP_MAX_RFMODS];
#endif

    BLST_S_HEAD(MixerList, BAPE_Mixer) mixerList;
    BLST_S_HEAD(MuxOutputList, BAPE_MuxOutput) muxOutputList;

#if BAPE_CHIP_MAX_PLLS > 0
    /* PLL Status */
    BAPE_AudioPll   audioPlls[BAPE_CHIP_MAX_PLLS];
#endif
#if BAPE_CHIP_MAX_NCOS > 0
    /* PLL Status */
    BAPE_AudioNco   audioNcos[BAPE_CHIP_MAX_NCOS];
#endif

    /* Groups */
    BAPE_SfifoGroupHandle     sfifoGroups[BAPE_CHIP_MAX_SFIFO_GROUPS];
    BAPE_DfifoGroupHandle     dfifoGroups[BAPE_CHIP_MAX_DFIFO_GROUPS];
    BAPE_SrcGroupHandle       srcGroups[BAPE_CHIP_MAX_SRC_GROUPS];
    BAPE_MixerGroupHandle     mixerGroups[BAPE_CHIP_MAX_MIXER_GROUPS];
#if BAPE_CHIP_MAX_LOOPBACK_GROUPS > 0
    BAPE_LoopbackGroupHandle  loopbackGroups[BAPE_CHIP_MAX_LOOPBACK_GROUPS];
#endif
#if BAPE_CHIP_MAX_DUMMYSINK_GROUPS > 0
    BAPE_DummysinkGroupHandle dummysinkGroups[BAPE_CHIP_MAX_DUMMYSINK_GROUPS];
#endif

    /* SRC Memory Heaps (DTV-Only) */
    BMEM_Heap_Handle srcCoefHeap;
    BMEM_Heap_Handle srcScratchHeap;

    /* Interrupts */
    BINT_CallbackHandle isrBfEsr1;
    BINT_CallbackHandle isrBfEsr2;
    BINT_CallbackHandle isrBfEsr3;
    BINT_CallbackHandle isrBfEsr4;
    BAPE_L3Interrupt sourceRbufFreemark[BAPE_CHIP_MAX_SFIFOS];
    BAPE_L3Interrupt destRbufFullmark[BAPE_CHIP_MAX_DFIFOS];
    BAPE_L3Interrupt destRbufOverflow[BAPE_CHIP_MAX_DFIFOS];

    /* Hardware resource allocation map */
    bool sfifoAllocated[BAPE_CHIP_MAX_SFIFOS];
    bool dfifoAllocated[BAPE_CHIP_MAX_DFIFOS];
    bool srcAllocated[BAPE_CHIP_MAX_SRCS];
#ifdef BAPE_CHIP_MAX_SRC_COEFF_CHUNKS
    bool srcCoeffAllocated[BAPE_CHIP_MAX_SRC_COEFF_CHUNKS];
#endif
    bool mixerAllocated[BAPE_CHIP_MAX_MIXERS];
    bool playbackAllocated[BAPE_CHIP_MAX_MIXER_PLAYBACKS];
    bool dummysinkAllocated[BAPE_CHIP_MAX_DUMMYSINKS];
#if BAPE_CHIP_MAX_LOOPBACKS > 0
    bool loopbackAllocated[BAPE_CHIP_MAX_LOOPBACKS];
#endif
#if BAPE_CHIP_MAX_FS > 0
    bool fsAllocated[BAPE_CHIP_MAX_FS];
#endif
    bool adaptRateAllocated[BAPE_CHIP_MAX_ADAPTRATE_CONTROLLERS];

    /* Usage Tracking for DP Playback */
    BAPE_FciId playbackFci[BAPE_CHIP_MAX_MIXER_PLAYBACKS];
    uint8_t playbackReferenceCount[BAPE_CHIP_MAX_MIXER_PLAYBACKS];

    /* Interrupts */
    BAPE_InterruptHandlers interrupts;

    bool bStandby; 
} BAPE_Device;

/***************************************************************************
Summary:
Generic Mixer Input Structure
***************************************************************************/
typedef struct BAPE_PathConnector
{
    BDBG_OBJECT(BAPE_PathConnector)
    struct BAPE_PathNode *pParent;

    /* Data Format Descriptor */
    BAPE_FMT_Descriptor format;
    bool useBufferPool;                             /* True if buffers should be pulled from the buffer pool */

    /* FW Source Information */
    unsigned branchId, stageId;                     /* Branch/Stage data will be sent from */
    BDSP_TaskCreateSettings *pTaskCreateSettings;   /* Task Creation Settings - Only valid during allocatePath call */
    BDSP_TaskHandle task;                           /* Task Handle - Becomes valid prior to start call and invalid prior to stop */

    /* Links to downstream connections */
    BLST_SQ_HEAD(ConnectionList, BAPE_PathConnection) connectionList;
    /* Buffer resources if useBufferBool is true */
    BAPE_BufferNode *pBuffers[BAPE_ChannelPair_eMax];
    /* Name */
    const char *pName;
} BAPE_PathConnector;

/***************************************************************************
Summary:
Init a connector
***************************************************************************/
#define BAPE_P_InitPathConnector(input, connectorParent) \
   do {\
         BKNI_Memset((input),0,sizeof(BAPE_PathConnector));\
         BDBG_OBJECT_SET((input),BAPE_PathConnector);\
         (input)->pParent=(connectorParent);\
         BLST_SQ_INIT(&(input)->connectionList); \
         BAPE_FMT_P_InitDescriptor(&(input)->format); \
         (input)->branchId = BAPE_BRANCH_ID_INVALID; \
         (input)->stageId = BAPE_STAGE_ID_INVALID; \
         (input)->pName = ""; \
   } while(0)

/***************************************************************************
Summary:
Path Node Type
***************************************************************************/
typedef enum BAPE_PathNodeType
{
    BAPE_PathNodeType_eDecoder,                     /* subtype=BAVC_AudioCompressionStd */
    BAPE_PathNodeType_ePlayback,
    BAPE_PathNodeType_eInputCapture,
    BAPE_PathNodeType_ePostProcessor,               /* subtype=BAPE_PostProcessorType */
    BAPE_PathNodeType_eEncoder,                     /* subtype=BAVC_AudioCompressionStd */
    BAPE_PathNodeType_eRfEncoder,                   /* subtype=BAPE_RfAudioEncoding */
    BAPE_PathNodeType_eMixer,                       /* subtype=BAPE_MixerType */
    BAPE_PathNodeType_eEqualizer,
    BAPE_PathNodeType_eMuxOutput,
    BAPE_PathNodeType_eEchoCanceller,               /* subtype=BAPE_EchoCancellerAlgorithm */
    BAPE_PathNodeType_eMax
} BAPE_PathNodeType;

/***************************************************************************
Summary:
Path Node State
***************************************************************************/
typedef enum BAPE_PathNodeState
{
    BAPE_PathNodeState_eIdle,
    BAPE_PathNodeState_eAllocatingPath,
    BAPE_PathNodeState_eAllocatedPath,
    BAPE_PathNodeState_eConfiguredPath,
    BAPE_PathNodeState_eStarted,    
    BAPE_PathNodeState_eMax
} BAPE_PathNodeState;

/***************************************************************************
Summary:
Post Processor Type
***************************************************************************/
typedef enum BAPE_PostProcessorType
{
    BAPE_PostProcessorType_eDdre,
    BAPE_PostProcessorType_eDolbyVolume,
    BAPE_PostProcessorType_eTruSurround,
    BAPE_PostProcessorType_eTruVolume,
    BAPE_PostProcessorType_eStudioSound,
    BAPE_PostProcessorType_eAudysseyAdv,
    BAPE_PostProcessorType_eAudysseyAbx,
    BAPE_PostProcessorType_e3dSurround,
    BAPE_PostProcessorType_eDolbyDigitalReencoder,
    BAPE_PostProcessorType_eAutoVolumeLevel,
    BAPE_PostProcessorType_eCustom,
    BAPE_PostProcessorType_eMax
} BAPE_PostProcessorType;


/***************************************************************************
Summary:
Maximum output connectors per node
***************************************************************************/
#define BAPE_MAX_CONNECTORS_PER_NODE (BAPE_ConnectorFormat_eMax)

/***************************************************************************
Summary:
Path Node
***************************************************************************/
typedef struct BAPE_PathNode
{
    BDBG_OBJECT(BAPE_PathNode)

    /* Node Specifics */
    BAPE_PathNodeType type;
    unsigned subtype;
    void *pHandle;              /* Pointer to actual object handle */
    BAPE_Handle deviceHandle;
    const char *pName;

    /* Node Status */
    BAPE_PathNodeState state;
    unsigned numStarts;         /* Number of times start has been called.  Stop must be called the same number of times.  */
    bool orphan;                /* Set to true if there is no eventual data consumption from this node.  It will be ignored if true. */

    /* Output Connectors from this node */
    unsigned numConnectors;
    BAPE_PathConnector connectors[BAPE_MAX_CONNECTORS_PER_NODE];

    /* List of input connections to this node */
    BLST_S_HEAD(UpstreamList, BAPE_PathConnection) upstreamList;
    BAPE_FMT_Capabilities inputCapabilities;    /* Supported input formats */

    /* Inter-Node Callbacks */
    BERR_Code (*allocatePathFromInput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    void      (*freePathFromInput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    BERR_Code (*allocatePathToOutput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    void      (*freePathToOutput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    BERR_Code (*configPathFromInput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    BERR_Code (*configPathToOutput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    BERR_Code (*startPathFromInput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    void      (*stopPathFromInput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    BERR_Code (*startPathToOutput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    void      (*stopPathToOutput)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    void      (*inputSampleRateChange_isr)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned newSampleRate);
    BERR_Code (*inputFormatChange)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
    void      (*inputMute)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, bool enabled);
    BERR_Code (*outputConnectionAdded)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
    void      (*outputConnectionRemoved)(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);

    /* InputPort->Node Callbacks */
    BERR_Code (*inputPortFormatChange_isr)(struct BAPE_PathNode *pNode, BAPE_InputPort port);

    /* Cleanup Callback */
    void      (*removeInput)(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);
} BAPE_PathNode;

/***************************************************************************
Summary:
Init a connector
***************************************************************************/
#define BAPE_P_InitPathNode(node, nodetype, nodesubtype, nConnectors, devHandle, handle) \
   do {\
         unsigned i;\
         BKNI_Memset((node),0,sizeof(BAPE_PathNode));\
         BDBG_OBJECT_SET((node),BAPE_PathNode);\
         (node)->pHandle=(handle);\
         (node)->type=(nodetype);\
         (node)->subtype=(nodesubtype);\
         (node)->numConnectors=(nConnectors);\
         (node)->deviceHandle=(devHandle);\
         BDBG_OBJECT_ASSERT((devHandle), BAPE_Device);\
         for ( i = 0; i < (nConnectors); i++ ) \
         { \
             BAPE_P_InitPathConnector(&((node)->connectors[i]), (node)); \
         } \
         BLST_S_INIT(&(node)->upstreamList); \
         BAPE_FMT_P_InitCapabilities(&(node)->inputCapabilities, NULL, NULL); \
   } while(0)

/***************************************************************************
Summary:
Output Path Status
***************************************************************************/
typedef struct BAPE_PathNodeOutputStatus
{
    unsigned numConnectors;
    struct
    {
        unsigned directConnections;      /* Number of connections at the first level */
        unsigned totalConnections;       /* Total number of downstream connections */
        unsigned totalBranches;          /* Total number of downstream branches */
        unsigned numValidOutputs;        /* Total number of non-orphan output nodes */
        unsigned numOutputs[BAPE_DataType_eMax];  /* Total number of outputs of each data type from this node. */
    } connectorStatus[BAPE_MAX_CONNECTORS_PER_NODE];

    unsigned activeConnectors;       /* Number of active connectors */
    unsigned directConnections;      /* Number of connections at the first level of all connectors */
    unsigned totalConnections;       /* Total number of downstream connections including all connectors */
    unsigned totalBranches;          /* Total number of downstream branches including all connectors */
    unsigned numValidOutputs;        /* Total number of non-orphan output nodes */
    unsigned numOutputs[BAPE_DataType_eMax];  /* Total number of outputs of each data type from this node. */
} BAPE_PathNodeOutputStatus;

/***************************************************************************
Summary:
Invalid Stream ID
***************************************************************************/
#define BAPE_STREAM_ID_INVALID (0xff)

/***************************************************************************
Summary:
Connection from an upstream connector to a downstream node
***************************************************************************/
typedef struct BAPE_PathConnection
{
    BDBG_OBJECT(BAPE_PathConnection)
    BAPE_PathConnector *pSource;
    BAPE_PathNode *pSink;
    BLST_S_ENTRY(BAPE_PathConnection) upstreamNode;
    BLST_SQ_ENTRY(BAPE_PathConnection) downstreamNode;

    /* Are the resources allocated? */
    bool resourcesAllocated;

    /* Data format descriptor */
    BAPE_FMT_Descriptor format;

    /* FMM Input Details */
    BAPE_FciIdGroup inputFciGroup;

    /* FMM Resources */
    BAPE_SfifoGroupHandle       sfifoGroup;
    BAPE_SrcGroupHandle         srcGroup;
    BAPE_MixerGroupHandle       mixerGroup;
    BAPE_DfifoGroupHandle       dfifoGroup;
    BAPE_LoopbackGroupHandle    loopbackGroup;

    /* Buffers for DSP mixer/Echo Canceller connections */
    BAPE_BufferNode *pLoopbackBuffers[BAPE_ChannelPair_eMax];
    BDSP_AF_P_sIO_BUFFER *pInterTaskIoDescriptor;
    void *pInterTaskIoBuffer;
    BDSP_AF_P_sIO_GENERIC_BUFFER *pInterTaskGenericDescriptor;
    void *pInterTaskGenericBuffer;

    /* DSP Mixer/Echo Canceller Input Handle */
    BDSP_TaskStageInputHandle taskInput;
} BAPE_PathConnection;

/***************************************************************************
Summary:
Timing Source Options
***************************************************************************/
typedef enum BAPE_MclkSource
{
    BAPE_MclkSource_eNone,
    BAPE_MclkSource_ePll0,
    BAPE_MclkSource_ePll1,
    BAPE_MclkSource_ePll2,
    BAPE_MclkSource_eHifidac0,
    BAPE_MclkSource_eHifidac1,
    BAPE_MclkSource_eHifidac2,
    BAPE_MclkSource_eNco0,
    BAPE_MclkSource_eNco1,
    BAPE_MclkSource_eNco2,
    BAPE_MclkSource_eNco3,
    BAPE_MclkSource_eNco4,
    BAPE_MclkSource_eMax
} BAPE_MclkSource;

#define BAPE_MCLKSOURCE_IS_PLL(mclkSource)  (mclkSource >= BAPE_MclkSource_ePll0     && mclkSource <= BAPE_MclkSource_ePll2)
#define BAPE_MCLKSOURCE_IS_DAC(mclkSource)  (mclkSource >= BAPE_MclkSource_eHifidac0 && mclkSource <= BAPE_MclkSource_eHifidac2)
#define BAPE_MCLKSOURCE_IS_NCO(mclkSource)  (mclkSource >= BAPE_MclkSource_eNco0     && mclkSource <= BAPE_MclkSource_eNco2)

/***************************************************************************
Summary:
Mixer Interface Structure to store callbacks for mixer functions.
***************************************************************************/
typedef struct BAPE_MixerInterface {
    void      (*destroy)            (BAPE_MixerHandle handle);
    BERR_Code (*start)              (BAPE_MixerHandle handle);
    void      (*stop)               (BAPE_MixerHandle handle);
    BERR_Code (*addInput)           (BAPE_MixerHandle handle, BAPE_Connector input, const BAPE_MixerAddInputSettings *pSettings);
    BERR_Code (*removeInput)        (BAPE_MixerHandle handle, BAPE_Connector input );
    BERR_Code (*removeAllInputs)    (BAPE_MixerHandle handle);
    BERR_Code (*addOutput)          (BAPE_MixerHandle handle, BAPE_OutputPort output );
    BERR_Code (*removeOutput)       (BAPE_MixerHandle handle, BAPE_OutputPort output );
    BERR_Code (*removeAllOutputs)   (BAPE_MixerHandle handle);
    BERR_Code (*getInputVolume)     (BAPE_MixerHandle handle, BAPE_Connector input, BAPE_MixerInputVolume *pVolume);
    BERR_Code (*setInputVolume)     (BAPE_MixerHandle handle, BAPE_Connector input, const BAPE_MixerInputVolume *pVolume);
    BERR_Code (*applyOutputVolume)  (BAPE_MixerHandle handle, BAPE_OutputPort output);
    BERR_Code (*setSettings)        (BAPE_MixerHandle hMixer, const BAPE_MixerSettings *pSettings);
} BAPE_MixerInterface ;

/***************************************************************************
Summary:
Mixer Structure
***************************************************************************/
typedef struct BAPE_Mixer
{
    BDBG_OBJECT(BAPE_Mixer)
    const BAPE_MixerInterface *interface;
    unsigned running;
    bool startedExplicitly;  /* true => mixer was started with BAPE_Mixer_Start()*/
    bool resourcesAllocated;
    bool restartPending;
    unsigned index;    
    unsigned numMixerGroups;
    unsigned fs;
    unsigned numOutputs;
    unsigned numOutputConnections;  /* Number of output connections.  Used when connecting a standard mixer to another FMM node such as eq or mixer. */
    BAPE_PathNode pathNode;
    BAPE_MclkSource mclkSource;
    BAPE_MixerGroupHandle mixerGroups[BAPE_CHIP_MAX_MIXERS];
    BAPE_Handle deviceHandle;
    BAPE_MixerSettings settings;
    BAPE_Connector master;
    BAPE_Connector inputs[BAPE_CHIP_MAX_MIXER_INPUTS];
    bool inputMuted[BAPE_CHIP_MAX_MIXER_INPUTS];
    bool inputRunning[BAPE_CHIP_MAX_MIXER_INPUTS];
    BAPE_MixerInputVolume inputVolume[BAPE_CHIP_MAX_MIXER_INPUTS];
#if BAPE_CHIP_MAX_DSP_MIXERS > 0
    BDSP_TaskHandle task;
    BDSP_TaskCreateSettings taskCreateSettings;
    BDSP_CIT_P_FwBranchInfo *pPrimaryBranch;
    BDSP_AF_P_sSINGLE_CIRC_BUFFER *pFeedbackBufferDescriptor;
    void *pFeedbackBuffer;
    BDSP_AF_P_sOpSamplingFreq sampleRateMap;
#endif
    BLST_S_HEAD(OutputList, BAPE_OutputPortObject) outputList;
    BLST_S_ENTRY(BAPE_Mixer) pllNode;
    BLST_S_ENTRY(BAPE_Mixer) ncoNode;
    BLST_S_ENTRY(BAPE_Mixer) node;
} BAPE_Mixer;

/***************************************************************************
Summary:
Output Port Types
***************************************************************************/
typedef enum BAPE_OutputPortType
{
    BAPE_OutputPortType_eI2sOutput,
    BAPE_OutputPortType_eI2sMultiOutput,
    BAPE_OutputPortType_eMaiOutput,
    BAPE_OutputPortType_eSpdifOutput,
    BAPE_OutputPortType_eDac,
    BAPE_OutputPortType_eDummyOutput,
    BAPE_OutputPortType_eOutputCapture,
    BAPE_OutputPortType_eMax
}BAPE_OutputPortType;

/***************************************************************************
Summary:
Generic Mixer Output Structure
***************************************************************************/
typedef struct BAPE_OutputPortObject
{
    BDBG_OBJECT(BAPE_OutputPort)
    BAPE_OutputPortType type;       /* type of output device this port belongs to */
    unsigned index;                 /* index with respect to type */
    void *pHandle;                  /* pointer to the specific output device */
    BAPE_MclkSource mclkOutput;     /* If this output generates an MCLK (e.g. DAC), which one is it? */
    bool muteInMixer;
    bool fsTiming;                  /* If this output slaves to an Fs source, this is true. */
    bool pllRequired;               /* If this output should always use a PLL this is true. */
    bool builtinNco;                /* If this is true, an output has its own NCO timing source (e.g. DAC). */

    BAPE_FMT_Capabilities capabilities; /* Input formats this output will accept */

    /* Primary APE_Mixer <-> Output Mapping */
    BAPE_MixerHandle mixer;                         /* NULL if not connected (set by mixer) */
    BLST_S_ENTRY(BAPE_OutputPortObject) node;       /* This is the node used in a mixer's linked list to manage a list of outputs (set by mixer) */

    BAPE_MixerGroupHandle sourceMixerGroup; /* (Set by mixer) */
    unsigned sourceMixerOutputIndex;        /* (Set by mixer) */
    BAPE_FciIdGroup sourceMixerFci;         /* (Set by mixer) */

    BAPE_OutputVolume volume;                       /* (Set by mixer) */

    void (*setTimingParams_isr)(BAPE_OutputPort output, unsigned sampleRate, BAVC_Timebase timebase);
    void (*setMclk_isr)(BAPE_OutputPort output, BAPE_MclkSource mclkSource, unsigned pllChannel, unsigned mclkFreqToFsRatio);
    void (*setFs)(BAPE_OutputPort output, unsigned fsNum);
    void (*setMute)(BAPE_OutputPort output, bool muted, bool sync);
    BERR_Code (*enable)(BAPE_OutputPort output);
    void      (*disable)(BAPE_OutputPort output);
    const char *pName;
} BAPE_OutputPortObject;

/***************************************************************************
Summary:
Init an output port
***************************************************************************/
#define BAPE_P_InitOutputPort(output, typ, indx, handle) \
   do {\
         unsigned _idx;\
         BKNI_Memset((output),0,sizeof(BAPE_OutputPortObject));\
         BDBG_OBJECT_SET((output),BAPE_OutputPort);\
         (output)->type = typ; \
         (output)->index = indx; \
         (output)->pHandle=handle;\
         for ( _idx = 0; _idx < BAPE_Channel_eMax; _idx++ ) { (output)->volume.volume[_idx] = 0x800000; }\
            BAPE_FciIdGroup_Init(&(output)->sourceMixerFci); \
   } while(0)

/***************************************************************************
Summary:
Input Port Types
***************************************************************************/
typedef enum BAPE_InputPortType
{
    BAPE_InputPortType_eI2s,
    BAPE_InputPortType_eMai,
    BAPE_InputPortType_eSpdif,
    BAPE_InputPortType_eRf,
    BAPE_InputPortType_eAdc,
    BAPE_InputPortType_eMax
}BAPE_InputPortType;

/***************************************************************************
Summary:
Generic InputPort Structure
***************************************************************************/
typedef struct BAPE_InputPortObject
{
    BDBG_OBJECT(BAPE_InputPort)
    BAPE_InputPortType type;    /* type of output device this port belongs to */   
    unsigned index;             /* index with respect to type */                   
    void *pHandle;              /* pointer to the specific output device */        
    BAPE_PathNode *pConsumer;                         /* Consumer path node */
    bool consumerAttaching;                           /* True if a consumer is in process of attaching */
    uint16_t streamId[BAPE_ChannelPair_eMax];         /* Input stream ID */
    BAPE_FMT_Descriptor format;                       /* Output data format */
    void (*enable)(BAPE_InputPort  inputPort);        /* Called when consumer starts */
    void (*disable)(BAPE_InputPort inputPort);        /* Called when consumer stops */
    BERR_Code (*consumerAttached_isr)(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer, BAPE_FMT_Descriptor *pFormat);       /* Called when a new consumer attaches */
    void      (*consumerDetached_isr)(BAPE_InputPort inputPort, BAPE_PathNode *pConsumer);       /* Called when a consumer detaches */    
    bool halted;                /* true => Input halted due to unhandled format change. */
    const char *pName;
} BAPE_InputPortObject;

/***************************************************************************
Summary:
Init an input port
***************************************************************************/
#define BAPE_P_InitInputPort(input, typ, indx, handle) \
   do {\
         unsigned i;\
         BKNI_Memset((input),0,sizeof(BAPE_InputPortObject));\
         BDBG_OBJECT_SET((input),BAPE_InputPort);\
         (input)->type = typ; \
         (input)->index = indx; \
         (input)->pHandle=handle;\
         BAPE_FMT_P_InitDescriptor(&(input)->format); \
         for ( i = 0; i < BAPE_ChannelPair_eMax; i++ ) { (input)->streamId[i] = BAPE_STREAM_ID_INVALID; }\
   } while(0)

/***************************************************************************
Summary:
Get the input port format descriptor
***************************************************************************/
void BAPE_InputPort_P_GetFormat_isr(
    BAPE_InputPort inputPort,
    BAPE_FMT_Descriptor *pFormat    /* [out] */
    );

/***************************************************************************
Summary:
Update the input port format descriptor
***************************************************************************/
BERR_Code BAPE_InputPort_P_SetFormat_isr(
    BAPE_InputPort inputPort,
    const BAPE_FMT_Descriptor *pNewFormat
    );

/***************************************************************************
Summary:
Tell the input port to enter the "halted" state.
***************************************************************************/
BERR_Code BAPE_InputPort_P_Halt_isr(
    BAPE_InputPort inputPort
    );

/***************************************************************************
Summary:
Attach a consumer to an input port
***************************************************************************/
BERR_Code BAPE_InputPort_P_AttachConsumer(
    BAPE_InputPort inputPort,
    BAPE_PathNode *pConsumer,
    BAPE_FMT_Descriptor *pInputFormat   /* [out] Current format */
    );

/***************************************************************************
Summary:
Detach a consumer from an input port
***************************************************************************/
BERR_Code BAPE_InputPort_P_DetachConsumer(
    BAPE_InputPort inputPort,
    BAPE_PathNode *pConsumer
    );

/***************************************************************************
Summary:
Get FCI Ids for Input Ports
***************************************************************************/
void BAPE_InputPort_P_GetFciIds(
    BAPE_InputPort inputPort,
    BAPE_FciIdGroup *pFciGroup      /* [out] */
    );

/***************************************************************************
Summary:
Convert IEC-61937 Burst Preamble C to a codec type.
***************************************************************************/
void BAPE_InputPort_P_BurstPreambleToCodec(
    uint32_t burstPreamble,
    BAVC_AudioCompressionStd *codec
    );

#ifdef BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR
/***************************************************************************
Summary:
Rinbguffer Stride
***************************************************************************/
#define BAPE_P_RINGBUFFER_STRIDE (BCHP_AUD_FMM_BF_CTRL_RINGBUF_1_RDADDR-BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR)

/***************************************************************************
Summary:
Helpers to get ringbuffer register addresses from source channel id
***************************************************************************/
#define BAPE_P_SFIFO_TO_RINBGUFFER(id) (2*(id))
#define BAPE_P_SFIFO_TO_RDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_WRADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_BASEADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_ENDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_FREEFULL_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_WRPOINT_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))

/***************************************************************************
Summary:
Helpers to get ringbuffer register addresses from destination channel id
***************************************************************************/
#define BAPE_P_DFIFO_TO_RINBGUFFER(id) ((2*(id)) + BAPE_P_SFIFO_TO_RINBGUFFER(BAPE_CHIP_MAX_SFIFOS))
#define BAPE_P_DFIFO_TO_RDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_RDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id)+BAPE_CHIP_MAX_SFIFOS)))
#define BAPE_P_DFIFO_TO_WRADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_WRADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id)+BAPE_CHIP_MAX_SFIFOS)))
#define BAPE_P_DFIFO_TO_BASEADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_BASEADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id)+BAPE_CHIP_MAX_SFIFOS)))
#define BAPE_P_DFIFO_TO_ENDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_ENDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id)+BAPE_CHIP_MAX_SFIFOS)))
#define BAPE_P_DFIFO_TO_FREEFULL_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_FREEFULL_MARK + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id)+BAPE_CHIP_MAX_SFIFOS)))
#define BAPE_P_DFIFO_TO_WRPOINT_REG(id) (BCHP_AUD_FMM_BF_CTRL_RINGBUF_0_START_WRPOINT + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id)+BAPE_CHIP_MAX_SFIFOS)))

#else
/* Newer 7429-style RDB */
/***************************************************************************
Summary:
Rinbguffer Stride
***************************************************************************/
#define BAPE_P_RINGBUFFER_STRIDE (BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR-BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR)

/***************************************************************************
Summary:
Helpers to get ringbuffer register addresses from source channel id
***************************************************************************/
#define BAPE_P_SFIFO_TO_RINBGUFFER(id) (2*(id))
#define BAPE_P_SFIFO_TO_RDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_WRADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_BASEADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_ENDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_FREEFULL_REG(id) (BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_FREEFULL_MARK + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_SFIFO_TO_WRPOINT_REG(id) (BCHP_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_START_WRPOINT + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))

/***************************************************************************
Summary:
Helpers to get ringbuffer register addresses from destination channel id
***************************************************************************/
#define BAPE_P_DFIFO_TO_RINBGUFFER(id) ((2*(id)) + BAPE_P_SFIFO_TO_RINBGUFFER(BAPE_CHIP_MAX_SFIFOS))
#define BAPE_P_DFIFO_TO_RDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_RDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_DFIFO_TO_WRADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_WRADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_DFIFO_TO_BASEADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_BASEADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_DFIFO_TO_ENDADDR_REG(id) (BCHP_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_ENDADDR + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_DFIFO_TO_FREEFULL_REG(id) (BCHP_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_FREEFULL_MARK + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#define BAPE_P_DFIFO_TO_WRPOINT_REG(id) (BCHP_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_START_WRPOINT + (2*BAPE_P_RINGBUFFER_STRIDE*((uint32_t)(id))))
#endif

/***************************************************************************
Summary:
Init the L3 interrupt handlers
***************************************************************************/
BERR_Code BAPE_P_InitInterrupts(
    BAPE_Handle handle
    );

/***************************************************************************
Summary:
Uninit the L3 interrupt handlers
***************************************************************************/
void BAPE_P_UninitInterrupts(
    BAPE_Handle handle
    );

/***************************************************************************
Summary:
Enable/Disable a source channel freemark interrupt
***************************************************************************/
BERR_Code BAPE_P_SetSourceChannelFreemarkInterrupt(
    BAPE_Handle handle,
    unsigned sourceChannelId,
    BINT_CallbackFunc callback_isr,
    void *pParam1,
    int param2
    );

/***************************************************************************
Summary:
Enable/Disable a destination channel fullmark interrupt
***************************************************************************/
BERR_Code BAPE_P_SetDfifoFullmarkInterrupt(
    BAPE_Handle handle,
    unsigned destChannelId,
    BINT_CallbackFunc callback_isr,
    void *pParam1,
    int param2
    );

/***************************************************************************
Summary:
Enable/Disable a destination channel overflow interrupt
***************************************************************************/
BERR_Code BAPE_P_SetDfifoOverflowInterrupt(
    BAPE_Handle handle,
    unsigned destChannelId,
    BINT_CallbackFunc callback_isr,
    void *pParam1,
    int param2
    );

/***************************************************************************
Summary:
Helper to get the SPDIF channel status code for a sample rate (IEC60958-3)
***************************************************************************/
unsigned BAPE_P_GetSampleRateCstatCode(unsigned sampleRate);

/***************************************************************************
Summary:
Create a "Standard" mixer
***************************************************************************/
BERR_Code BAPE_StandardMixer_P_Create(
    BAPE_Handle deviceHandle,
    const BAPE_MixerSettings *pSettings,
    BAPE_MixerHandle *pHandle               /* [out] */
    );

/***************************************************************************
Summary:
Create a DSP mixer
***************************************************************************/
BERR_Code BAPE_DspMixer_P_Create(
    BAPE_Handle deviceHandle,
    const BAPE_MixerSettings *pSettings,
    BAPE_MixerHandle *pHandle               /* [out] */
    );

/***************************************************************************
Summary:
Find the index of a mixer input connector
***************************************************************************/
#define BAPE_MIXER_INPUT_INDEX_INVALID ((unsigned)-1)
unsigned BAPE_Mixer_P_FindInputIndex(BAPE_MixerHandle mixer, BAPE_Connector input);

/***************************************************************************
Summary:
Return the default mixer input volume
***************************************************************************/
void BAPE_Mixer_P_GetDefaultInputVolume(BAPE_MixerInputVolume *pVolume);

/***************************************************************************
Summary:
Apply the volume setting to a mixer's output
***************************************************************************/
BERR_Code BAPE_Mixer_P_ApplyOutputVolume(BAPE_MixerHandle mixer, BAPE_OutputPort output);

/***************************************************************************
Summary:
Print information (using BDBG_LOG) about a BAPE_PathNode.
***************************************************************************/
BERR_Code BAPE_Mixer_P_PrintNodeInfo( BAPE_PathNode *pPathNode, int level, int index);

/***************************************************************************
Summary:
Print a mixer-centric representation of the audio filter graph (for all 
mixers). 
***************************************************************************/
BERR_Code BAPE_Mixer_P_PrintMixers(BAPE_Handle deviceHandle);

/***************************************************************************
Summary:
Print a representation of the audio filter graph (for all downstream 
PathNodes). 
***************************************************************************/
BERR_Code BAPE_Mixer_P_PrintDownstreamNodes(BAPE_PathNode *pPathNode);

/***************************************************************************
Summary:
Print a representation of the audio filter graph (for all downstream 
PathNodes). 
***************************************************************************/
const char *BAPE_Mixer_P_MclkSourceToText(BAPE_MclkSource mclkSource);

/***************************************************************************
Summary:
Get a mixer's output data type 
***************************************************************************/
#define BAPE_Mixer_P_GetOutputDataType_isr BAPE_Mixer_P_GetOutputDataType
#define BAPE_Mixer_P_GetOutputDataType(hMix) ((hMix)->pathNode.connectors[0].format.type)

/***************************************************************************
Summary: 
Get a mixer's output data type 
***************************************************************************/
#define BAPE_Mixer_P_GetOutputFormat_isr BAPE_Mixer_P_GetOutputFormat
#define BAPE_Mixer_P_GetOutputFormat(hMix) ((const BAPE_FMT_Descriptor *)&(hMix)->pathNode.connectors[0].format)

/***************************************************************************
Summary: 
Get a mixer's output sample rate
***************************************************************************/
#define BAPE_Mixer_P_GetOutputSampleRate_isr BAPE_Mixer_P_GetOutputSampleRate
#define BAPE_Mixer_P_GetOutputSampleRate(hMix) ((hMix)->pathNode.connectors[0].format.sampleRate)

/***************************************************************************
Summary: 
Get a mixer's output data type based on the widest of all inputs
***************************************************************************/
BERR_Code BAPE_Mixer_P_DetermineOutputDataType(BAPE_MixerHandle handle, BAPE_DataType *pDataType /* [out] */);

/***************************************************************************
Summary:
Attach a mixer to a PLL
***************************************************************************/
void BAPE_P_AttachMixerToPll(BAPE_MixerHandle mixer, BAPE_Pll pll);

/***************************************************************************
Summary:
Detach a mixer from a PLL
***************************************************************************/
void BAPE_P_DetachMixerFromPll(BAPE_MixerHandle mixer, BAPE_Pll pll);

/***************************************************************************
Summary:
Update PLL sample rate
***************************************************************************/
BERR_Code BAPE_P_UpdatePll_isr(BAPE_Handle handle, BAPE_Pll pll);

/***************************************************************************
Summary:
Attach a mixer to a NCO
***************************************************************************/
void BAPE_P_AttachMixerToNco(BAPE_MixerHandle mixer, BAPE_Nco nco);

/***************************************************************************
Summary:
Detach a mixer from a NCO
***************************************************************************/
void BAPE_P_DetachMixerFromNco(BAPE_MixerHandle mixer, BAPE_Nco nco);

/***************************************************************************
Summary:
Update NCO sample rate
***************************************************************************/
BERR_Code BAPE_P_UpdateNco_isr(BAPE_Handle handle, BAPE_Nco nco);

/***************************************************************************
Summary:
Allocate buffers from the resource pool
***************************************************************************/
BERR_Code BAPE_P_AllocateBuffers(
    BAPE_Handle deviceHandle,
    const BAPE_FMT_Descriptor *pDesc, 
    BAPE_BufferNode *pBuffers[BAPE_ChannelPair_eMax]
    );

/***************************************************************************
Summary:
Release buffers to the resource pool
***************************************************************************/
void BAPE_P_FreeBuffers(
    BAPE_Handle deviceHandle,
    BAPE_BufferNode *pBuffers[BAPE_ChannelPair_eMax]
    );

/*************************************************************************** 
Summary:
Allocate FMM Resource
***************************************************************************/
typedef enum BAPE_FmmResourceType
{
    BAPE_FmmResourceType_eSfifo,
    BAPE_FmmResourceType_eDfifo,
    BAPE_FmmResourceType_eSrc,
    BAPE_FmmResourceType_eMixer,
    BAPE_FmmResourceType_ePlayback,
    BAPE_FmmResourceType_eDummysink,
    BAPE_FmmResourceType_eLoopback,
    BAPE_FmmResourceType_eFs,
    BAPE_FmmResourceType_eAdaptiveRate,
    BAPE_FmmResourceType_eMax
} BAPE_FmmResourceType;

/*************************************************************************** 
Summary:
Allocate FMM Resource
***************************************************************************/
#if 1
#define BAPE_P_AllocateFmmResource(h,r,n,f) BAPE_P_AllocateFmmResource_tagged((h),(r),(n),(f),__FILE__,__LINE__);
BERR_Code BAPE_P_AllocateFmmResource_tagged(BAPE_Handle handle, BAPE_FmmResourceType resourceType, unsigned numChannelPairs, unsigned *pFirstResource, const char *pFile, unsigned line);
#else
BERR_Code BAPE_P_AllocateFmmResource(
    BAPE_Handle handle, 
    BAPE_FmmResourceType resourceType, 
    unsigned numChannelPairs, 
    unsigned *pFirstResource
    );
#endif

/*************************************************************************** 
Summary:
Free FMM Resource
***************************************************************************/
void BAPE_P_FreeFmmResource(
    BAPE_Handle handle, 
    BAPE_FmmResourceType resourceType, 
    unsigned numChannelPairs, 
    unsigned firstResource
    );

/*************************************************************************** 
Summary:
Release all unused path resources.
***************************************************************************/
void BAPE_P_ReleaseUnusedPathResources(BAPE_Handle handle);

/***************************************************************************
Summary:
Allocate Input Buffers from the resource pool
***************************************************************************/
BERR_Code BAPE_P_AllocateInputBuffers(BAPE_Handle handle, BAPE_Connector input);

/***************************************************************************
Summary:
Free Input Buffers to the resource pool
***************************************************************************/
void BAPE_P_FreeInputBuffers(BAPE_Handle handle, BAPE_Connector input);

/***************************************************************************
Summary:
Determine if a MuxOutput object has been started 
***************************************************************************/
bool BAPE_MuxOutput_P_IsRunning(BAPE_MuxOutputHandle handle);

#if defined BCHP_AUD_FMM_MS_CTRL_REG_START
/***************************************************************************
Summary:
Microsequencer FW version
***************************************************************************/
extern const uint32_t g_BAPE_MS_FirmwareVersion[4];

/***************************************************************************
Summary:
Microsequencer FW
***************************************************************************/
#include "bchp_aud_fmm_ms_ctrl.h"
extern const uint32_t g_BAPE_MS_Firmware[BCHP_AUD_FMM_MS_CTRL_USEQ_INSTi_ARRAY_END + 1];
#endif

/***************************************************************************
Summary:
Decoder Handle
***************************************************************************/
typedef struct BAPE_Decoder
{
    BDBG_OBJECT(BAPE_Decoder)
    BAPE_Handle deviceHandle;
    unsigned index;
    BAPE_PathNode node;
    char name[10]; /* Decoder %u */

    /* Basic State Information */
    bool independentDelay;
    bool passthrough;
    bool simul;
    bool stereoOnMultichannel;
    bool stereoOnCompressed;

    BAPE_PathNodeOutputStatus outputStatus;
    BAPE_DecoderState state;
    BAPE_DecoderSettings settings;
    BAPE_DecoderStartSettings startSettings;
    BAVC_XptContextMap contextMap;
    BAPE_DecoderTsmSettings tsmSettings;    

    /* Codec-specific settings */
    /* TODO: Codec settings should be stored in the individual structures and not the full union to save space. */
    BAPE_DecoderCodecSettings ac3Settings, ac3PlusSettings;
    BAPE_DecoderCodecSettings aacSettings, aacPlusSettings;
    BAPE_DecoderCodecSettings wmaProSettings;
    BAPE_DecoderCodecSettings dtsSettings;
    BAPE_DecoderCodecSettings adpcmSettings;
    BAPE_DecoderInterruptHandlers interrupts;

    /* DSP Task Information */
    BDSP_TaskHandle task;
    BDSP_TaskCreateSettings taskCreateSettings;
    BDSP_CIT_P_FwBranchInfo primaryBranch;
    unsigned dspIndex;
    unsigned sampleRate;
    BDSP_AF_P_sOpSamplingFreq sampleRateMap;
    unsigned mode;
    unsigned dsolaBranch, dsolaStage;
    BDSP_AudioBitRateChangeInfo bitRateInfo;

    /* DFIFO Input Information */
    BAPE_FMT_Descriptor inputPortFormat;
    BAPE_DfifoGroupHandle inputDfifoGroup;
    BAPE_BufferNode *pInputBuffers[BAPE_ChannelPair_eMax];

    /* Required for Multi-stream Decoding  */
    bool fwMixerMaster;
    BAPE_DolbyDigitalReencodeHandle ddre;
    BAPE_MixerHandle fwMixer;    

    /* Ancillary Data Handling */
    BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pAncDataBufferDescriptor;
    void *pAncDataDspBuffer;
    size_t ancDataBufferSize;
    void *pAncDataHostBuffer;
    size_t ancDataBufferRead;
    size_t ancDataBufferWrite;
    bool ancDataBufferReadWrap;
    bool ancDataBufferWriteWrap;
    bool ancDataInit;
} BAPE_Decoder;

/***************************************************************************
Summary:
Initialize the ancillary data buffer descriptor prior to start
***************************************************************************/
void BAPE_Decoder_P_InitAncillaryDataBuffer(BAPE_DecoderHandle hDecoder);

/***************************************************************************
Summary:
Set a decoder's sample rate
***************************************************************************/
void BAPE_Decoder_P_SetSampleRate_isr(BAPE_DecoderHandle handle, unsigned sampleRate);

/***************************************************************************
Summary:
Get default codec specifics for the DSP codecs
***************************************************************************/
void BAPE_Decoder_P_GetDefaultCodecSettings(BAPE_DecoderHandle handle);

/***************************************************************************
Summary:
Apply codec settings to DSP
***************************************************************************/
BERR_Code BAPE_Decoder_P_ApplyCodecSettings(BAPE_DecoderHandle handle);

/***************************************************************************
Summary:
Get codec status from DSP
***************************************************************************/
BERR_Code BAPE_Decoder_P_GetCodecStatus(BAPE_DecoderHandle handle, BAPE_DecoderStatus *pStatus);

/***************************************************************************
Summary:
Propagate settings between decoder and DDRE
***************************************************************************/
BERR_Code BAPE_DolbyDigitalReencode_P_SettingsChanged(
    BAPE_DolbyDigitalReencodeHandle handle, 
    BAPE_DecoderHandle decoder
    );

/***************************************************************************
Summary:
Playback Handle Structure
***************************************************************************/
typedef struct BAPE_Playback
{
    BDBG_OBJECT(BAPE_Playback)
    BAPE_Handle hApe;
    BMEM_Heap_Handle hHeap;
    unsigned index;
    BAPE_PathNode node;
    BAPE_PathConnection *pMaster;
    BAPE_PlaybackStartSettings startSettings;
    void *pBuffer[BAPE_Channel_eMax];
    BAPE_PlaybackSettings settings;
    unsigned bufferSize;
    unsigned numBuffers;
    unsigned threshold;
    unsigned bufferDepth;   /* Required for pre-start buffer fills (used in looparound mode) */
    BAPE_PlaybackInterruptHandlers interrupts;
    bool running;
    char name[12]; /* Playback %d */
} BAPE_Playback;

/***************************************************************************
Summary:
Input Capture Handle Structure
***************************************************************************/
typedef struct BAPE_InputCapture
{
    BDBG_OBJECT(BAPE_InputCapture)
    BAPE_Handle deviceHandle;
    unsigned index;
    BAPE_PathNode node;
    BAPE_PathConnection *pMaster;
    BAPE_InputCaptureOpenSettings openSettings;
    BAPE_InputCaptureStartSettings startSettings;
    BAPE_InputCaptureInterruptHandlers interrupts;
    BAPE_DfifoGroupHandle dfifoGroup;
    BAPE_PathConnection *pMasterConnection;         /* Master for SFIFO master/slave designation */
    BAPE_FMT_Descriptor inputPortFormat;
    bool running;
    char name[16];  /* InputCapture %d */
} BAPE_InputCapture;

/***************************************************************************
Summary:
Echo Canceller Handle Structure
***************************************************************************/
typedef struct BAPE_EchoCanceller
{
    BDBG_OBJECT(BAPE_EchoCanceller)
    BAPE_Handle deviceHandle;
    BAPE_PathNode localNode, remoteNode;    /* Need two nodes to distinguish the local vs. remote input paths.  They don't behave the same way. */
    BAPE_EchoCancellerSettings settings;
    BAPE_SpeexEchoCancellerSettings speexSettings;
    BAPE_Connector localInput, remoteInput;
    bool localStarted, remoteStarted;

    /* Inter-task communication details */
    BDSP_AF_P_sIO_BUFFER *pInterTaskIoDescriptor;
    void *pInterTaskIoBuffer;
    BDSP_AF_P_sIO_GENERIC_BUFFER *pInterTaskGenericDescriptor;
    void *pInterTaskGenericBuffer;
} BAPE_EchoCanceller;

/***************************************************************************
Summary:
Get the address of a mixer's config register
***************************************************************************/
uint32_t BAPE_P_GetMixerConfigAddress(unsigned mixerId);

/***************************************************************************
Summary:
Invalid Fs handle
***************************************************************************/
#define BAPE_FS_INVALID ((unsigned)-1)

/***************************************************************************
Summary:
Allocate an Fs timing source
***************************************************************************/
unsigned BAPE_P_AllocateFs(BAPE_Handle handle);

/***************************************************************************
Summary:
Free an Fs timing source
***************************************************************************/
void BAPE_P_FreeFs(BAPE_Handle handle, unsigned fs);

/***************************************************************************
Summary:
Set the timing parameters for an Fs timing source
***************************************************************************/
void BAPE_P_SetFsTiming_isr(
    BAPE_Handle handle, 
    unsigned fsIndex, 
    BAPE_MclkSource mclkSource, 
    unsigned pllChannel, 
    unsigned mclkFreqToFsRatio
    );

/***************************************************************************
Summary:
Map User settings to DSP for TruVolume.  Required in both StudioSound 
and TruVolume. 
***************************************************************************/
BERR_Code BAPE_TruVolume_P_ConvertSettingsToDsp(
    const BAPE_TruVolumeSettings *pSettings, 
    BDSP_Raaga_Audio_TruVolumeUserConfig *pUserConfig
    );

/***************************************************************************
Summary:
Map User settings to DSP for TruSurroundHd.  Required in both StudioSound 
and TruSurroundHd. 
***************************************************************************/
BERR_Code BAPE_TruSurroundHd_P_ConvertSettingsToDsp(
    const BAPE_TruSurroundHdSettings *pSettings, 
    unsigned numChannelPairs,
    BDSP_Raaga_Audio_TruSurrndHDConfigParams *pUserConfig
    );

#if BAPE_CHIP_MAX_SPDIF_OUTPUTS > 0 || BAPE_CHIP_MAX_MAI_OUTPUTS > 0
/***************************************************************************
Summary:
SPDIF Channel Status Bits
***************************************************************************/
typedef struct BAPE_Spdif_P_ChannelStatusBits
{
    uint32_t bits[3];
}BAPE_Spdif_P_ChannelStatusBits;

/***************************************************************************
Summary:
Setup Channel Status Bits for SPDIF or HDMI outputs
***************************************************************************/
void BAPE_P_MapSpdifChannelStatusToBits_isr(
    BAPE_OutputPort output,
    const BAPE_SpdifChannelStatus *pChannelStatus,
    BAPE_Spdif_P_ChannelStatusBits *pBits           /* [out] */
    );
#endif

/* These must be after definition of path types above */
#include "bape_dsp_utils_priv.h"
#include "bape_path_priv.h"
#include "bape_fmm_priv.h"

#endif /* #ifndef BAPE_PRIV_H_ */

