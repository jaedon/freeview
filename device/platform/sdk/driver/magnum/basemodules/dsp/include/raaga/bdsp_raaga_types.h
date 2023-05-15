/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_types.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 11/26/12 1:18p $
 *
 * Module Description: Raaga Types header
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/raaga/bdsp_raaga_types.h $
 * 
 * Hydra_Software_Devel/9   11/26/12 1:18p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/8   3/23/12 11:12a gprasad
 * SW7435-5: Including the dsp dma header file to make the burst size
 * programming effective for 7435
 * 
 * Hydra_Software_Devel/7   1/27/12 1:20p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/6   1/6/12 10:54a gprasad
 * SW7435-5: Adding changes for modified RDB for 7435
 * 
 * Hydra_Software_Devel/5   1/3/12 2:24p ashoky
 * SW7435-5: Adding support for 2nd DSP into mainline.
 * 
 * Hydra_Software_Devel/4   11/24/11 6:55p ashoky
 * SW7429-18: Merging 7429 rdb related changes to mainline.
 * 
 * Hydra_Software_Devel/3   4/20/11 1:10p jgarrett
 * SW7425-392: Removing BDSP_ProcessingType enum
 * 
 * Hydra_Software_Devel/2   4/6/11 4:56p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/10   4/5/11 11:28p jgarrett
 * SW35330-35: Merge to main branch
 * 
 * Hydra_Software_Devel/SW35330-35/2   4/6/11 11:15a jgarrett
 * SW35330-35: Adding 35233
 * 
 * Hydra_Software_Devel/SW35330-35/1   4/5/11 7:14p jgarrett
 * SW35330-35: PCM Playback working on 35230
 * 
 * Hydra_Software_Devel/9   3/5/11 4:53a srajapur
 * SW7422-314 : [7422] Adding RDB Independence Feature in BASE DSP
 * Codebase
 * 
 * Hydra_Software_Devel/8   1/20/11 8:12a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/7   1/19/11 7:16a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/6   1/19/11 2:25a gautamk
 * SW7422-191: Adding comment header
 * 
 * Hydra_Software_Devel/2   1/13/11 6:00a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * 
 ***************************************************************************/
#ifndef BDSP_RAAGA_TYPES_H__
#define BDSP_RAAGA_TYPES_H__
#include "bdsp_types.h"
#include "bkni.h"
#include "bstd.h"



/*------------------------- RDB INCLUDE FILES --------------------------------*/
#include "bchp_common.h"
#if 0
#include "bchp_int_id_aio_inth.h"
#endif
#include "bchp_int_id_raaga_dsp_inth.h"
#include "bchp_int_id_raaga_dsp_fw_inth.h"
#include "bchp_raaga_dsp_dma.h"
#include "bchp_raaga_dsp_esr_si.h"
#include "bchp_raaga_dsp_fw_cfg.h"

#ifdef BCHP_RAAGA_DSP_FW_CFG_MASTER_IT_POINTER
    #define RAAGA_DSP_FW_CFG_PAGE_START BCHP_RAAGA_DSP_FW_CFG_MASTER_IT_POINTER
#elif defined BCHP_RAAGA_DSP_FW_CFG_SW_UNDEFINED_SECTION0_i_ARRAY_BASE
    #define RAAGA_DSP_FW_CFG_PAGE_START BCHP_RAAGA_DSP_FW_CFG_SW_UNDEFINED_SECTION0_i_ARRAY_BASE
#else
    #error "Cannot find first register in the fw_cfg page"
#endif

#include "bchp_raaga_dsp_fw_inth.h"
#include "bchp_raaga_dsp_inth.h"
#include "bchp_raaga_dsp_mem_subsystem.h"
#include "bchp_raaga_dsp_misc.h"
#include "bchp_raaga_dsp_peri_dbg_ctrl.h"
#include "bchp_raaga_dsp_peri_sw.h"
#include "bchp_raaga_dsp_rgr.h"
#include "bchp_raaga_dsp_timers.h"
#include "bchp_common.h"

#if 0
#include "bchp_aio_misc.h"
#include "bchp_aio_inth.h"
#include "bchp_aio_intd0.h"

#include "bchp_aud_fmm_iop_ctrl.h"
#include "bchp_aud_fmm_iop_esr.h"
#include "bchp_aud_fmm_misc.h"
#include "bchp_aud_fmm_ms_ctrl.h"
#include "bchp_aud_fmm_ms_esr.h"
#include "bchp_aud_fmm_op_ctrl.h"
#include "bchp_aud_fmm_op_esr.h"
#include "bchp_aud_fmm_src_ctrl0.h"
#include "bchp_aud_fmm_src_esr0.h"
#include "bchp_aud_fmm_bf_ctrl.h"
#include "bchp_aud_fmm_bf_esr0_d0.h"
#include "bchp_aud_fmm_bf_esr0_h.h"
#include "bchp_aud_fmm_bf_esr1_d0.h"
#include "bchp_aud_fmm_bf_esr1_h.h"
#include "bchp_aud_fmm_bf_esr2_d0.h"
#include "bchp_aud_fmm_bf_esr2_h.h"
#include "bchp_aud_fmm_dp_ctrl0.h"
#include "bchp_hifidac_ctrl0.h"
#include "bchp_hifidac_esr0.h"
#include "bchp_hifidac_rm0.h"
#endif 

/****** Defines for DSP1 ***********/
#ifdef BCHP_RAAGA_DSP_RGR_1_REG_START
#include "bchp_raaga_dsp_rgr_1.h"
#endif 

#if defined BCHP_RAAGA_DSP_INTH_1_REG_START
#include "bchp_int_id_raaga_dsp_inth_1.h"
#include "bchp_int_id_raaga_dsp_fw_inth_1.h"
#endif

#ifdef BCHP_CLKGEN_REG_START

#include "bchp_clkgen.h"
#endif

#ifdef BCHP_AUD_FMM_PLL0_REG_START
#include "bchp_aud_fmm_pll0.h"
#endif
#ifdef BCHP_AUD_FMM_PLL1_REG_START
#include "bchp_aud_fmm_pll1.h"
#endif
#ifdef BCHP_AUD_FMM_PLL2_REG_START

#include "bchp_aud_fmm_pll2.h"
#endif

#ifdef BCHP_HIFIDAC_CTRL1_REG_START
#include "bchp_hifidac_ctrl1.h"
#include "bchp_hifidac_esr1.h"
#include "bchp_hifidac_rm1.h"
#endif

#ifdef BCHP_HIFIDAC_CTRL2_REG_START
#include "bchp_hifidac_ctrl2.h"
#include "bchp_hifidac_esr2.h"
#include "bchp_hifidac_rm2.h"
#endif

#ifdef BCHP_AUDIO0_PLL_REG_START
#include "bchp_audio0_pll.h"
#endif
#ifdef BCHP_AUDIO1_PLL_REG_START
#include "bchp_audio1_pll.h"
#endif
#ifdef BCHP_PLL_AUDIO0_REG_START
#include "bchp_pll_audio0.h"
#endif
#ifdef BCHP_PLL_AUDIO1_REG_START
#include "bchp_pll_audio1.h"
#endif

/* Max number of DSPs present in SOC */
#if defined BCHP_RAAGA_DSP_RGR_1_REG_START
#define BDSP_RAAGA_MAX_DSP 2
#else
#define BDSP_RAAGA_MAX_DSP 1
#endif

#ifdef BCHP_RAAGA_DSP_DMA_1_REG_START
#include "bchp_raaga_dsp_dma_1.h"
#endif 


/* Max Post-processing branches & stages */

#define BDSP_RAAGA_MAX_STAGE_PER_BRANCH (7+3)/*MAX PP stage added externally + MAX internal stage added*/
                                                                               /*(Decode + SRC + DSOLA)*/
#define BDSP_RAAGA_MAX_BRANCH   3

/* Max settings for video encoder on Raaga */
#define	BDSP_RAAGA_MAX_BRANCH_VIDEO_ENCODE				1
#define BDSP_RAAGA_MAX_STAGE_PER_BRANCH_VIDEO_ENCODE	2	/* 1 might be needed for genCdbItb */

#define	BDSP_RAAGA_MAX_BRANCH_SCM				1
#define BDSP_RAAGA_MAX_STAGE_PER_BRANCH_SCM		1	
/***************************************************************************
Summary:
	Supported Audio Outputs.

Description:
	This enum defines the different types of supported Audio Outputs.
	
See Also:
	
****************************************************************************/
typedef enum BDSP_OutputPort
{
	BDSP_OutputPort_eSpdif = 0,
	BDSP_OutputPort_eDac0,
	BDSP_OutputPort_eI2s0,
	BDSP_OutputPort_eI2s1,
	BDSP_OutputPort_eI2s2,
	BDSP_OutputPort_eDac1,
	BDSP_OutputPort_eMai,
	BDSP_OutputPort_eFlex,
	BDSP_OutputPort_eRfMod,
	BDSP_OutputPort_eArc,
    BDSP_OutputPort_eI2s3,
    BDSP_OutputPort_eI2s4,
    BDSP_OutputPort_eSpdif1,
    BDSP_OutputPort_eI2s5,
    BDSP_OutputPort_eI2s6,
    BDSP_OutputPort_eI2s7,
    BDSP_OutputPort_eI2s8,
	BDSP_OutputPort_eDac2,
	BDSP_OutputPort_eI2s9,
    BDSP_OutputPort_eMaiMulti0,
    BDSP_OutputPort_eMaiMulti1,
    BDSP_OutputPort_eMaiMulti2,
    BDSP_OutputPort_eMaiMulti3,
    BDSP_OutputPort_eDummySink0,
    BDSP_OutputPort_eDummySink1,    
    BDSP_OutputPort_eDummySink2,
    BDSP_OutputPort_eDummySink3,
    BDSP_OutputPort_eMax
}BDSP_OutputPort;


/***************************************************************************
Summary:
	Capture Input Ports.

Description:
	It enumerates various PCM Capture Input Ports present in Raptor.	
	
See Also:
	
**************************************************************************/
typedef enum BDSP_CapInputPort
{
    BDSP_CapInputPort_eIntCapPort0 = 0, /* Internal Capture port 0 */
    BDSP_CapInputPort_eIntCapPort1,     /* Internal Capture port 1 */
    BDSP_CapInputPort_eIntCapPort2,     /* Internal Capture port 2 */
    BDSP_CapInputPort_eIntCapPort3,     /* Internal Capture port 3 */
    BDSP_CapInputPort_eExtI2s0,         /* External I2S Capture port */
    BDSP_CapInputPort_eRfI2s,           /* I2S Port taking in RF data */
    BDSP_CapInputPort_eSpdif,           /* SPDIF Capture port */
    BDSP_CapInputPort_eHdmi,            /* HDMI */
    BDSP_CapInputPort_eAdc,             /* ADC capture port */
    BDSP_CapInputPort_eIntCapPort4,     /* Internal Capture port 4 */
    BDSP_CapInputPort_eIntCapPort5,     /* Internal Capture port 5 */
    BDSP_CapInputPort_eIntCapPort6,     /* Internal Capture port 6 */
    BDSP_CapInputPort_eIntCapPort7,     /* Internal Capture port 7 */
    BDSP_CapInputPort_eMax              /* Invalid/last Entry */
} BDSP_CapInputPort;

#endif



