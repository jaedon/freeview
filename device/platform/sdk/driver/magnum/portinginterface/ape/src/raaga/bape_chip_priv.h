/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_chip_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/47 $
 * $brcm_Date: 9/27/12 10:17a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_chip_priv.h $
 * 
 * Hydra_Software_Devel/47   9/27/12 10:17a mpeteri
 * SW7445-19: merge 7445 APE changes to main
 * 
 * Hydra_Software_Devel/SW7445-19/2   9/26/12 10:56a mpeteri
 * SW7445-19: Add support for PLL Macros.  Switch to 128 fs base
 * 
 * Hydra_Software_Devel/SW7445-19/1   8/17/12 5:49p mpeteri
 * SW7445-19: Add additional PLL and NCO for 7445
 * 
 * Hydra_Software_Devel/46   5/14/12 3:16p jgarrett
 * SW7425-2695: Adding full 42-bit STC
 * 
 * Hydra_Software_Devel/45   5/3/12 5:05p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/3   5/3/12 3:45p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/44   4/27/12 2:18p jgarrett
 * SW7425-2690: Adding numVcxos to capabilities
 * 
 * Hydra_Software_Devel/SW7425-2075/2   4/12/12 1:57p jgarrett
 * SW7425-2075: Adding HBR input
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/43   3/5/12 9:22a jgarrett
 * SW7425-2455: Removing ADC and RF Inputs
 * 
 * Hydra_Software_Devel/42   1/18/12 11:21a gskerl
 * SW7429-18: Removed erroneous #error when there are no HDMI or SPDIF
 * inputs.
 * 
 * Hydra_Software_Devel/41   1/17/12 6:31p gskerl
 * SW7429-18: Added conditional definitions for
 * BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY and
 * BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN.
 * 
 * Hydra_Software_Devel/40   1/13/12 3:28p gskerl
 * SW7429-18: Added BAPE_CHIP_MAI_INPUT_TYPE_IS_xxx macros
 * 
 * Hydra_Software_Devel/39   12/5/11 7:02p gskerl
 * SW7429-18: Define sample rate converter type... either
 * BAPE_CHIP_SRC_TYPE_IS_LEGACY or BAPE_CHIP_SRC_TYPE_IS_IIR.
 * 
 * Hydra_Software_Devel/39   12/5/11 7:02p gskerl
 * SW7429-18: Define sample rate converter type... either
 * BAPE_CHIP_SRC_TYPE_IS_LEGACY or BAPE_CHIP_SRC_TYPE_IS_IIR.
 * 
 * Hydra_Software_Devel/39   12/5/11 6:50p gskerl
 * SW7429-18: Define sample rate converter type... either
 * BAPE_CHIP_SRC_TYPE_IS_LEGACY or BAPE_CHIP_SRC_TYPE_IS_IIR.
 * 
 * Hydra_Software_Devel/38   12/1/11 6:40p gskerl
 * SW7429-18: Only define BAPE_CHIP_MAX_SRC_COEFF_CHUNKS when it's
 * dependencies are defined.
 * 
 * Hydra_Software_Devel/37   12/1/11 3:43p jgarrett
 * SW7429-18: Correcting STC address selection for 7429
 * 
 * Hydra_Software_Devel/36   11/30/11 6:28p jgarrett
 * SW7429-18: Revising coefficient numbers for 7429
 * 
 * Hydra_Software_Devel/35   11/30/11 4:26p jgarrett
 * SW7429-18: Adding stubs for SPDIF/MAI inputs on 7429
 * 
 * Hydra_Software_Devel/34   11/29/11 12:03p gskerl
 * SW7429-18: Fixed compile warning (and bug) #ifdef --> #if defined.
 * 
 * Hydra_Software_Devel/33   11/28/11 6:32p gskerl
 * SW7429-18: Added check for
 * BCHP_AUD_MISC_SEROUT_SEL_HDMI_RX_ARC_ENABLE_MASK to decide if
 * BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS should be defined
 * 
 * Hydra_Software_Devel/32   11/14/11 3:22p gskerl
 * SW7429-18: Merging 7429 changes back to main branch.
 * 
 * Hydra_Software_Devel/SW7429-18/7   11/4/11 4:42p jgarrett
 * SW7429-18: Adding SPDIF Output for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/6   10/27/11 3:21p jgarrett
 * SW7429-18: Adding I2S Output for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/5   10/26/11 12:44p jgarrett
 * SW7429-18: Merging latest changes from main branch
 * 
 * Hydra_Software_Devel/SW7429-18/4   10/26/11 11:43a jgarrett
 * SW7429-18: Adding I2S Input for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/3   10/25/11 5:34p jgarrett
 * SW7429-18: Adding HDMI output support for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/2   10/25/11 10:15a jgarrett
 * SW7429-18: Adding NCO support for 7429
 * 
 * Hydra_Software_Devel/SW7429-18/1   10/21/11 6:29p jgarrett
 * SW7429-18: Initial compileable version for 7429
 * 
 * Hydra_Software_Devel/31   10/19/11 4:56p jgarrett
 * SW7231-321: Fixed clockgen mux for 7231 B0
 * 
 * Hydra_Software_Devel/30   9/2/11 3:53p sgadara
 * SWDTV-6627: [35233] Refine the SRC coefficient memory allocation
 * 
 * Hydra_Software_Devel/29   8/26/11 9:50p sgadara
 * SWDTV-6627: [35233] Add Equalizer support
 * 
 * Hydra_Software_Devel/28   8/12/11 4:18p venkatr
 * SWDTV-6584 : [35233] ADC input code cleanup
 * 
 * Hydra_Software_Devel/27   8/11/11 1:16p jgarrett
 * SWDTV-6584: Making includes of audadc and spdif_rcvr register headers
 * conditional on presence of the required blocks
 * 
 * Hydra_Software_Devel/26   8/10/11 9:28a venkatr
 * SWDTV-6584 : [35233] Add ADC Input for APE
 * 
 * Hydra_Software_Devel/25   7/8/11 6:38p jgarrett
 * SWDTV-6760: Adding I2sMultiOutput
 * 
 * Hydra_Software_Devel/24   7/8/11 4:32p gskerl
 * SW7552-72: Added support for NCO/Mclkgen audio clock sources
 * 
 * Hydra_Software_Devel/23   6/9/11 3:40p gskerl
 * SW7552-37: Set BAPE_CHIP_MAX_RFMODS based on RDB defs
 * 
 * Hydra_Software_Devel/22   5/31/11 6:37p jgarrett
 * SW7425-406: Adding initial DSP mixer code
 * 
 * Hydra_Software_Devel/21   5/12/11 12:11p gskerl
 * SW7422-354: Fixed to support APE compilation on 35230
 * 
 * Hydra_Software_Devel/20   5/3/11 7:00p gskerl
 * SW7422-354: First attempt at adding support for the audio return
 * channel
 * 
 * Hydra_Software_Devel/19   4/18/11 10:13p gskerl
 * SW7425-364: Added BAPE_Pll_EnableExternalMclk() API to APE, then called
 * it from NEXUS_AudioModule_EnableExternalMclk()
 * 
 * Hydra_Software_Devel/18   4/16/11 12:15p jgarrett
 * SW7425-371: Removing tab characters
 * 
 * Hydra_Software_Devel/17   4/11/11 5:54p jgarrett
 * SWDTV-6305: Adding ADC/RF Inputs for DTV
 * 
 * Hydra_Software_Devel/16   4/6/11 1:24a jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/3   4/6/11 11:15a jgarrett
 * SW35330-35: Adding 35233
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/5/11 7:13p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 12:50p jgarrett
 * SW35330-35: FMM Abstraction refactoring to support DTV
 * 
 * Hydra_Software_Devel/15   4/5/11 6:53p gskerl
 * SW7552-28: Updated comment.
 * 
 * Hydra_Software_Devel/14   3/24/11 7:55p gskerl
 * SW7422-146: Improved audio reference clock selection logic to handle
 * RDB differences for the 7231
 * 
 * Hydra_Software_Devel/13   3/24/11 10:28a jgarrett
 * SW7422-364: Fixing compilation errors on systems without HDMI or SPDIF
 * input
 * 
 * Hydra_Software_Devel/12   3/23/11 11:03a piyushg
 * SW7422-146: Make MAX_HDMI and MAX_SDPIF recvr marcos independent of
 * BCHP_74xx macros
 * 
 * Hydra_Software_Devel/11   3/10/11 7:03p jgarrett
 * SW7422-146: Refactored DFIFO code, added support for input capture from
 * compressed/multichannel
 * 
 * Hydra_Software_Devel/10   3/9/11 4:47p piyushg
 * SW7422-146: Initial checkin for HDMI and SPDIF inout ports.
 * 
 * Hydra_Software_Devel/9   2/28/11 1:55p gskerl
 * SW7422-146: Increased BAPE_CHIP_MAX_DACS from 1 to 2.
 * 
 * Hydra_Software_Devel/8   2/28/11 1:28p jgarrett
 * SW7422-146: Filter graph reworked to remove mixer dependencies
 * 
 * Hydra_Software_Devel/7   2/28/11 12:22p gskerl
 * SW7422-146: Changed BAPE_CHIP_MAX_RFMODS from 1 to 0 (because the 7422
 * doesn't have any RF mods).
 * 
 * Hydra_Software_Devel/6   2/9/11 5:28p gskerl
 * SW7422-146:Added "#define BAPE_BASE_PLL_TO_FS_RATIO   256" which
 * defines ratio of
 * the PLL's channel 0 frequency to the "base" sample rate.
 * 
 * Hydra_Software_Devel/5   2/4/11 12:43p gskerl
 * SW7422-146:Modified the counting of I2S outputs so that it will handle
 * 7422/7425 I2S register names.
 * 
 * Hydra_Software_Devel/4   1/19/11 2:58p jgarrett
 * SW7422-146: Initial decode/passthrough of ac3
 * 
 * Hydra_Software_Devel/3   1/6/11 2:33p jgarrett
 * SW7422-146: Adding initial input capture API
 * 
 * Hydra_Software_Devel/2   12/17/10 3:58p jgarrett
 * SW7422-146: Nexus APE integration on 7422
 * 
 * Hydra_Software_Devel/1   12/16/10 4:05p jgarrett
 * SW7422-146: Initial compilable APE for 7422
 ***************************************************************************/

#ifndef BAPE_CHIP_PRIV_H_
#define BAPE_CHIP_PRIV_H_

#include "bchp_common.h"
#include "bchp_aud_fmm_bf_ctrl.h"
#include "bchp_aud_fmm_dp_ctrl0.h"
#include "bchp_aud_fmm_src_ctrl0.h"
#if defined BCHP_AUD_FMM_OP_CTRL_REG_START
/* Older-style RDB only */
#include "bchp_aud_fmm_op_ctrl.h"
#include "bchp_aud_fmm_iop_ctrl.h"
#endif
#if defined BCHP_AUD_FMM_OP_MCLKGEN_REG_START
    #include "bchp_aud_fmm_op_mclkgen.h"
#endif /* defined BCHP_AUD_FMM_OP_MCLKGEN_REG_START */

#ifdef BCHP_AIO_MISC_REG_START
#include "bchp_aio_misc.h"
#include "bchp_aud_fmm_misc.h"
#else
#include "bchp_aud_misc.h"
#endif

#ifdef BCHP_AUD_FMM_IOP_MISC_REG_START
#include "bchp_aud_fmm_iop_misc.h"
#endif

#define BAPE_CHIP_MAX_DECODERS (BAPE_CHIP_MAX_SFIFOS)
#define BAPE_CHIP_MAX_PLAYBACKS (BAPE_CHIP_MAX_SFIFOS)
#define BAPE_CHIP_MAX_INPUT_CAPTURES (BAPE_CHIP_MAX_DFIFOS)
#define BAPE_CHIP_MAX_DSP_MIXERS (1)                            /* Allow 1 DSP mixer */

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_TRANSCODER)
/* For A/V Transcoding of Live streaming */
#define BAPE_CHIP_MAX_DSP_TASKS (3+BAPE_CHIP_MAX_DSP_MIXERS+4)
#else
#define BAPE_CHIP_MAX_DSP_TASKS (3+BAPE_CHIP_MAX_DSP_MIXERS)    /* 3 decoders + 1 FW Mixer */
#endif

/* Max STCs */
#ifdef BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_END
/* Legacy RDB */
#define BAPE_CHIP_MAX_STCS (BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_END)
#ifdef BCHP_AUD_FMM_MISC_STC_LOWERi_ARRAY_BASE
#define BAPE_CHIP_HAS_42BIT_STC (1)
#define BAPE_CHIP_GET_STC_ADDRESS(idx) (BCHP_AUD_FMM_MISC_STC_LOWERi_ARRAY_BASE + ((BCHP_AUD_FMM_MISC_STC_LOWERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
#define BAPE_CHIP_GET_STC_UPPER_ADDRESS(idx) (BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_BASE + ((BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
#else
#define BAPE_CHIP_GET_STC_ADDRESS(idx) (BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_BASE + ((BCHP_AUD_FMM_MISC_STC_UPPERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
#endif
#else
/* 7429 style RDB */
#define BAPE_CHIP_MAX_STCS (BCHP_AUD_MISC_STC_UPPERi_ARRAY_END)

#ifdef BCHP_AUD_MISC_STC_LOWERi_ARRAY_BASE
#define BAPE_CHIP_HAS_42BIT_STC (1)
#define BAPE_CHIP_GET_STC_ADDRESS(idx) (BCHP_AUD_MISC_STC_LOWERi_ARRAY_BASE + ((BCHP_AUD_MISC_STC_LOWERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
#define BAPE_CHIP_GET_STC_UPPER_ADDRESS(idx) (BCHP_AUD_MISC_STC_UPPERi_ARRAY_BASE + ((BCHP_AUD_MISC_STC_UPPERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
#else
#define BAPE_CHIP_GET_STC_ADDRESS(idx) (BCHP_AUD_MISC_STC_UPPERi_ARRAY_BASE + ((BCHP_AUD_MISC_STC_UPPERi_ARRAY_ELEMENT_SIZE/8)*(idx)))
#endif
#endif

/* Check for DACs */
#if defined BCHP_HIFIDAC_CTRL2_REG_START || defined BCHP_HIFIDAC_CTRL_2_REG_START
#define BAPE_CHIP_MAX_DACS (3)
#elif defined BCHP_HIFIDAC_CTRL1_REG_START || defined BCHP_HIFIDAC_CTRL_1_REG_START
#define BAPE_CHIP_MAX_DACS (2)
#elif defined BCHP_HIFIDAC_CTRL0_REG_START || defined BCHP_HIFIDAC_CTRL_0_REG_START
#define BAPE_CHIP_MAX_DACS (1)
#endif

/* Check for the number of I2S outputs... */
#if defined BCHP_AUD_FMM_OP_CTRL_I2SS1_CFG || defined BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_1_REG_START
#define BAPE_CHIP_MAX_I2S_OUTPUTS (2)
#elif defined BCHP_AUD_FMM_OP_CTRL_I2SS0_CFG || defined BCHP_AUD_FMM_IOP_OUT_I2S_STEREO_0_REG_START
#define BAPE_CHIP_MAX_I2S_OUTPUTS (1)
/* There must not be any I2S outputs */
#else
#define BAPE_CHIP_MAX_I2S_OUTPUTS (0)
#endif

/* Check for I2S Multi Outputs */
#define BAPE_CHIP_MAX_I2S_MULTI_OUTPUTS (0)

/* Check for I2S Inputs */
#if defined BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG1 || defined BCHP_AUD_FMM_IOP_IN_I2S_STEREO_1_REG_START
#define BAPE_CHIP_MAX_I2S_INPUTS (2)
#elif defined BCHP_AUD_FMM_IOP_CTRL_I2SIN_CFG0 || defined BCHP_AUD_FMM_IOP_IN_I2S_STEREO_0_REG_START
#define BAPE_CHIP_MAX_I2S_INPUTS (1)
#else
#define BAPE_CHIP_MAX_I2S_INPUTS (0)
#endif

/* MAI Inputs */
#if defined BCHP_HDMI_RCVR_CTRL_REG_START
#include "bchp_hdmi_rcvr_ctrl.h"
#include "bchp_spdif_rcvr_ctrl.h"
    #define BAPE_CHIP_MAX_MAI_INPUTS (1)
    #define BAPE_CHIP_MAI_INPUT_TYPE_IS_LEGACY 1     /* 1=>true: This is a Legacy (7422/7425) type of MAI/HDMI input */
    #if defined BCHP_SPDIF_RCVR_CTRL_CONFIG_HBR_MODE_MASK
        #define BAPE_CHIP_MAI_INPUT_HBR_SUPPORT 1    /* 7435 added support for HBR input */
    #endif
#elif defined BCHP_AUD_FMM_IOP_IN_HDMI_0_REG_START
    #define BAPE_CHIP_MAX_MAI_INPUTS (1)
    #define BAPE_CHIP_MAI_INPUT_TYPE_IS_IOPIN 1      /* 1=>true: This is a newer (IOP IN) (7429) type of MAI/HDMI input */
    #define BAPE_CHIP_MAI_INPUT_HBR_SUPPORT 1        /* New chips with HDMI input support HBR */
#endif

/* SPDIF Inputs */
#if defined BCHP_SPDIF_RCVR_CTRL_REG_START
    #define BAPE_CHIP_MAX_SPDIF_INPUTS (1)
    #define BAPE_CHIP_SPDIF_INPUT_TYPE_IS_LEGACY 1     /* 1=>true: This is a Legacy (7422/7425) type of SPDIF input */
#elif defined BCHP_AUD_FMM_IOP_IN_SPDIF_0_REG_START
    #define BAPE_CHIP_MAX_SPDIF_INPUTS (1)
    #define BAPE_CHIP_SPDIF_INPUT_TYPE_IS_IOPIN 1      /* 1=>true: This is a newer (IOP IN) (7429) type of SPDIF input */
#endif

#if defined BCHP_RFM_SYSCLK_REG_START
#define BAPE_CHIP_MAX_RFMODS (1)
#else
#define BAPE_CHIP_MAX_RFMODS (0)
#endif

#if defined BCHP_AUD_FMM_OP_CTRL_SPDIF_CFG_0 || defined BCHP_AUD_FMM_IOP_OUT_SPDIF_0_REG_START
#define BAPE_CHIP_MAX_SPDIF_OUTPUTS (1)
#endif

#if defined BCHP_AUD_FMM_OP_CTRL_MAI_FORMAT || defined BCHP_AUD_FMM_IOP_OUT_MAI_0_REG_START
#define BAPE_CHIP_MAX_MAI_OUTPUTS (1)
#endif

#if defined  BCHP_AUD_FMM_MISC_SEROUT_SEL_SPDIF_OUT_1_ENABLE_MASK || defined BCHP_AUD_MISC_SEROUT_SEL_HDMI_RX_ARC_ENABLE_MASK
#define BAPE_CHIP_MAX_AUDIO_RETURN_CHANNELS (1)
#endif

#define BAPE_CHIP_MAX_OUTPUT_CAPTURES (BAPE_CHIP_MAX_LOOPBACKS)

/* PLL Details */
#if   defined BCHP_AUD_FMM_PLL2_REG_START || defined BCHP_AUD_FMM_IOP_PLL_2_REG_START
#define BAPE_CHIP_MAX_PLLS (3)
#elif defined BCHP_AUD_FMM_PLL1_REG_START || defined BCHP_AUD_FMM_IOP_PLL_1_REG_START
#define BAPE_CHIP_MAX_PLLS (2)
#elif defined BCHP_AUD_FMM_PLL0_REG_START || defined BCHP_AUD_FMM_IOP_PLL_0_REG_START
#define BAPE_CHIP_MAX_PLLS (1)
#else
/* No PLLs */
#define BAPE_CHIP_MAX_PLLS (0)
#endif

/* NCO Details */
#if   defined BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_4_CONTROL || defined BCHP_AUD_FMM_IOP_NCO_4_REG_START
#define BAPE_CHIP_MAX_NCOS (5)
#elif defined BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_3_CONTROL || defined BCHP_AUD_FMM_IOP_NCO_3_REG_START
#define BAPE_CHIP_MAX_NCOS (4)
#elif defined BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_2_CONTROL || defined BCHP_AUD_FMM_IOP_NCO_2_REG_START
#define BAPE_CHIP_MAX_NCOS (3)
#elif defined BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_1_CONTROL || defined BCHP_AUD_FMM_IOP_NCO_1_REG_START
#define BAPE_CHIP_MAX_NCOS (2)
#elif defined BCHP_AUD_FMM_OP_MCLKGEN_MCLK_GEN_0_CONTROL || defined BCHP_AUD_FMM_IOP_NCO_0_REG_START
#define BAPE_CHIP_MAX_NCOS (1)
#else
/* No NCOs */
#endif

#define BAPE_BASE_PLL_TO_FS_RATIO   128     /* PLL channel 0 runs at 128 * "base" Fs */

/* SFIFO Details */
#define BAPE_CHIP_MAX_SFIFOS (BCHP_AUD_FMM_BF_CTRL_SOURCECH_CFGi_ARRAY_END+1)

/* DFIFO Details */
#ifdef BCHP_AUD_FMM_BF_CTRL_DESTCH_CFG0
#define BAPE_CHIP_MAX_DFIFOS (1)  /* Required because the RDB defs change with only one channel */
#else
#define BAPE_CHIP_MAX_DFIFOS (BCHP_AUD_FMM_BF_CTRL_DESTCH_CFGi_ARRAY_END+1)
#endif
#if defined(BCHP_AUD_FMM_BF_CTRL_DESTCH_CFG0_CAPTURE_MODE_SHIFT) || defined(BCHP_AUD_FMM_BF_CTRL_DESTCH_CFGi_CAPTURE_MODE_SHIFT)
#define BAPE_CHIP_DFIFO_SUPPORTS_16BIT_CAPTURE 1
#endif

/* SRC Details */
#define BAPE_CHIP_MAX_SRCS (BCHP_AUD_FMM_SRC_CTRL0_STRM_CFGi_ARRAY_END+1)

#if defined BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_BASE
    #define BAPE_CHIP_SRC_TYPE_IS_IIR 1     /* 1=>true: This is an IIR-type of SRC */
    #if BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_END == 487
        /* SRC Coefficient Memory */
        #define BAPE_CHIP_P_SRC_IIR_CHUNK_BASE          0
        #define BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS          11
        #define BAPE_CHIP_P_SRC_IIR_CHUNK_END           (BAPE_CHIP_P_SRC_IIR_CHUNK_BASE + BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS - 1)
        #define BAPE_CHIP_P_MAX_IIR_FILTERS_PER_SRC     8
        #define BAPE_CHIP_P_MAX_IIR_COEFF_PER_SRC       (5 * BAPE_CHIP_P_MAX_IIR_FILTERS_PER_SRC)
        #define BAPE_CHIP_P_MAX_SRC_IIR_COEFF_PER_CHUNK BAPE_CHIP_P_MAX_IIR_COEFF_PER_SRC
        #define BAPE_CHIP_P_TOTAL_SRC_IIR_COEFF         (BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS * BAPE_CHIP_P_MAX_SRC_IIR_COEFF_PER_CHUNK)

        #define BAPE_CHIP_P_SRC_LIN_CHUNK_BASE          (BAPE_CHIP_P_SRC_IIR_CHUNK_END + 1)
        #define BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS          16
        #define BAPE_CHIP_P_SRC_LIN_CHUNK_END           (BAPE_CHIP_P_SRC_LIN_CHUNK_BASE + BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS - 1)
        #define BAPE_CHIP_P_NUM_LIN_COEFF_PER_SRC       3
        #define BAPE_CHIP_P_NUM_LIN_COEFF_PER_CHUNK     BAPE_CHIP_P_NUM_LIN_COEFF_PER_SRC
        #define BAPE_CHIP_P_TOTAL_LIN_COEFF             (BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS * BAPE_CHIP_P_NUM_LIN_COEFF_PER_CHUNK)
        #define BAPE_CHIP_MAX_SRC_COEFF_CHUNKS          (BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS + BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS)

    #elif BCHP_AUD_FMM_SRC_CTRL0_COEFFi_ARRAY_END == 231
        #define BAPE_CHIP_P_SRC_IIR_CHUNK_BASE          0
        #define BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS          4
        #define BAPE_CHIP_P_SRC_IIR_CHUNK_END           (BAPE_CHIP_P_SRC_IIR_CHUNK_BASE + BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS - 1)
        #define BAPE_CHIP_P_MAX_IIR_FILTERS_PER_SRC     8
        #define BAPE_CHIP_P_MAX_IIR_COEFF_PER_SRC       (5 * BAPE_CHIP_P_MAX_IIR_FILTERS_PER_SRC)
        #define BAPE_CHIP_P_MAX_SRC_IIR_COEFF_PER_CHUNK BAPE_CHIP_P_MAX_IIR_COEFF_PER_SRC
        #define BAPE_CHIP_P_TOTAL_SRC_IIR_COEFF         (BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS * BAPE_CHIP_P_MAX_SRC_IIR_COEFF_PER_CHUNK)

        #define BAPE_CHIP_P_SRC_LIN_CHUNK_BASE          (BAPE_CHIP_P_SRC_IIR_CHUNK_END + 1)
        #define BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS          16
        #define BAPE_CHIP_P_SRC_LIN_CHUNK_END           (BAPE_CHIP_P_SRC_LIN_CHUNK_BASE + BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS - 1)
        #define BAPE_CHIP_P_NUM_LIN_COEFF_PER_SRC       3
        #define BAPE_CHIP_P_NUM_LIN_COEFF_PER_CHUNK     BAPE_CHIP_P_NUM_LIN_COEFF_PER_SRC
        #define BAPE_CHIP_P_TOTAL_LIN_COEFF             (BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS * BAPE_CHIP_P_NUM_LIN_COEFF_PER_CHUNK)
        #define BAPE_CHIP_MAX_SRC_COEFF_CHUNKS          (BAPE_CHIP_P_MAX_SRC_IIR_CHUNKS + BAPE_CHIP_P_MAX_SRC_LIN_CHUNKS)
    #else
        #error unsupported number of SRC coefficients
    #endif
#elif defined BCHP_AUD_FMM_SRC_CTRL0_COEFF2X_i_ARRAY_BASE
    #define BAPE_CHIP_SRC_TYPE_IS_LEGACY 1     /* 1=>true: This is an legacy type of SRC */
#else
    #error "Unknown Sample Rate Converter type"
#endif

/* Mixer Details */
#if BCHP_CHIP == 7408
#define BAPE_CHIP_MAX_MIXERS (6)                /* On the 7408, this is 6, others have more -- This must match the HW for coefficient loading. */
#define BAPE_CHIP_MAX_MIXER_INPUTS (4)          /* On the 7408, only four inputs are supported per mixer.  Most other chips is 8. */
#else
#if defined BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_END
/* 7429-style chips define both of these using RDB arrays */
#define BAPE_CHIP_MAX_MIXERS (BCHP_AUD_FMM_DP_CTRL0_MIXER_CONFIGi_ARRAY_END+1)
#define BAPE_CHIP_MAX_MIXER_INPUTS (BCHP_AUD_FMM_DP_CTRL0_MIXER0_INPUT_CONFIGi_ARRAY_END+1)
#elif defined BCHP_AUD_FMM_DP_CTRL0_MIXER11_CONFIG
/* 7425-style mixer block */
#define BAPE_CHIP_MAX_MIXERS (12)
#define BAPE_CHIP_MAX_MIXER_INPUTS (8)
#else
/* Legacy 8-mixer block */
#define BAPE_CHIP_MAX_MIXERS (8)
#define BAPE_CHIP_MAX_MIXER_INPUTS (8)
#endif
#endif
#define BAPE_CHIP_MAX_MIXER_OUTPUTS (2)
#define BAPE_CHIP_MAX_MIXER_PLAYBACKS (1+BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_END-BCHP_AUD_FMM_DP_CTRL0_PB_FCI_IDi_ARRAY_START)

/* Dummysinks */
#ifdef BCHP_AUD_FMM_IOP_DUMMYSINK_0_REG_START
#include "bchp_aud_fmm_iop_dummysink_0.h"
#define BAPE_CHIP_MAX_DUMMYSINKS (BCHP_AUD_FMM_IOP_DUMMYSINK_0_STREAM_CFG_0_i_ARRAY_END+1)
#else
#define BAPE_CHIP_MAX_DUMMYSINKS (4)
#endif

/* Loopbacks */
#if BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG0
#define BAPE_CHIP_MAX_LOOPBACKS (1)             /* On the 7408, this is 1, others have more */
#elif defined BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFG
#define BAPE_CHIP_MAX_LOOPBACKS (4)
#elif defined BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFGi_ARRAY_END
#define BAPE_CHIP_MAX_LOOPBACKS (BCHP_AUD_FMM_IOP_CTRL_LOOPBACK_CFGi_ARRAY_END+1)
#elif defined BCHP_AUD_FMM_IOP_LOOPBACK_0_REG_START
#include "bchp_aud_fmm_iop_loopback_0.h"
#define BAPE_CHIP_MAX_LOOPBACKS (BCHP_AUD_FMM_IOP_LOOPBACK_0_STREAM_CFG_i_ARRAY_END+1)
#endif

#ifdef BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_FSi_ARRAY_END
/* FS Timing Sources */
#define BAPE_CHIP_MAX_FS (BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_FSi_ARRAY_END+1)
#endif

/* External MCLK Outputs */
#if defined BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_END
#define BAPE_CHIP_MAX_EXT_MCLKS (BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_END+1)
#elif defined BCHP_AUD_FMM_IOP_MISC_MCLK_CFG_i_ARRAY_END
#define BAPE_CHIP_MAX_EXT_MCLKS (BCHP_AUD_FMM_IOP_MISC_MCLK_CFG_i_ARRAY_END+1)
#endif

#define BAPE_CHIP_MAX_OUTPUT_PORTS (BCHP_SHIFT(AUD_FMM_OP_CTRL_ENABLE_STATUS, reserved0))    /* Total output ports  */

#if defined BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_END
#define BAPE_CHIP_MAX_ADAPTRATE_CONTROLLERS (BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_CFGi_ARRAY_END+1)
#elif defined BCHP_AUD_FMM_BF_CTRL_ADAPTRATE_7_CFG
#define BAPE_CHIP_MAX_ADAPTRATE_CONTROLLERS (8)                  /* New chips provide 8 controllers */
#else
#define BAPE_CHIP_MAX_ADAPTRATE_CONTROLLERS (4)                  /* Legacy chips only provided 4 controllers */
#endif

#ifdef BCHP_AUD_FMM_IOP_CTRL_REG_START
#define BAPE_CHIP_MAX_IOP_STREAMS (BAPE_CHIP_MAX_OUTPUT_PORTS + BAPE_CHIP_MAX_LOOPBACKS + BAPE_CHIP_MAX_DUMMYSINKS)
#else
/* The "stream" concept went away on newer chips like 7429 */
#define BAPE_CHIP_MAX_IOP_STREAMS 0
#endif

#if BAPE_CHIP_MAX_MAI_OUTPUTS > 0
    #if defined BDSP_MLP_SUPPORT || defined BDSP_DTSHD_SUPPORT || BAPE_CHIP_MAI_INPUT_HBR_SUPPORT
        #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_16X_BUFFERS (1)    /* One HBR buffer */
        #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_4X_BUFFERS (0)
    #elif defined BDSP_MS10_SUPPORT || defined BDSP_DDP_SUPPORT
        #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_16X_BUFFERS (0)
        #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_4X_BUFFERS (1)     /* One 4x buffer for AC3+ */
    #else
        #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_16X_BUFFERS (0)
        #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_4X_BUFFERS (0)
    #endif
#else
    #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_16X_BUFFERS (0)
    #define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_4X_BUFFERS (0)
#endif

#define BAPE_CHIP_DEFAULT_NUM_COMPRESSED_BUFFERS (1)    /* By default we have one compressed buffer */
#define BAPE_CHIP_DEFAULT_NUM_PCM_BUFFERS (4)           /* 5.1 + one stereo */
#define BAPE_CHIP_MAX_PATH_DELAY (128)
#define BAPE_CHIP_BYTES_PER_PCM_SAMPLE (4)
#define BAPE_CHIP_BYTES_PER_PCM_SAMPLE_PAIR (2*BAPE_CHIP_BYTES_PER_PCM_SAMPLE)
#define BAPE_CHIP_BYTES_PER_COMPRESSED_SAMPLE (2)
#define BAPE_CHIP_INTERLEAVE_DSP_SAMPLES (0)

#define BAPE_CHIP_MAX_SFIFO_GROUPS (BAPE_CHIP_MAX_SFIFOS)
#define BAPE_CHIP_MAX_DFIFO_GROUPS (BAPE_CHIP_MAX_DFIFOS)
#define BAPE_CHIP_MAX_SRC_GROUPS (BAPE_CHIP_MAX_SRCS)
#define BAPE_CHIP_MAX_MIXER_GROUPS (BAPE_CHIP_MAX_MIXERS)
#define BAPE_CHIP_MAX_LOOPBACK_GROUPS (BAPE_CHIP_MAX_LOOPBACKS)
#define BAPE_CHIP_MAX_DUMMYSINK_GROUPS (BAPE_CHIP_MAX_DUMMYSINKS)

/* And finally, here are some things that aren't in the RDB */

/* Define values for the PLL_AUDIO<n>_REFERENCE_CLOCK fields of the
 * CLKGEN_INTERNAL_MUX_SELECT register
 */
#ifdef BCHP_CLKGEN_INTERNAL_MUX_SELECT
#if (BCHP_CHIP==7231 && BCHP_VER == BCHP_VER_A0)
    #define BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Fixed  1
    #define BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo0  0
#else /* for others  (7422, 7425, 7344, 7346, 7358)  */
    #define BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Fixed  0
    #define BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo0  1
    #define BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo1  2
    #define BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo2  3
#endif

#if   defined BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo3
#define BAPE_CHIP_MAX_VCXOS (4)
#elif defined BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo2
#define BAPE_CHIP_MAX_VCXOS (3)
#elif defined BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo1
#define BAPE_CHIP_MAX_VCXOS (2)
#elif defined BCHP_CLKGEN_INTERNAL_MUX_SELECT_PLLAUDIO0_REFERENCE_CLOCK_Vcxo0
#define BAPE_CHIP_MAX_VCXOS (1)
#endif

#endif



#endif /* #ifndef BAPE_CHIP_PRIV_H_ */

