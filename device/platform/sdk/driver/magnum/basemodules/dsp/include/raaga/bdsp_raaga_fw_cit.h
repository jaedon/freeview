/***************************************************************************
 *     Copyright (c) 2004-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fw_cit.h $
 * $brcm_Revision: Hydra_Software_Devel/SW7429-1116/1 $
 * $brcm_Date: 7/25/14 2:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/include/raaga/bdsp_raaga_fw_cit.h $
 * 
 * Hydra_Software_Devel/SW7429-1116/1   7/25/14 2:20p ananthan
 * SW7429-1116:Error return issue in case of using five audio output
 * [Adding support for 6 output ports]
 * 
 * Hydra_Software_Devel/21   11/26/12 1:17p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/20   9/7/12 12:17p gprasad
 * SW7425-3061: Moving sample raate LUT generation out of CIT
 * 
 * Hydra_Software_Devel/19   7/3/12 3:01p ananthan
 * SW7425-3182: Correcting passthrough type assignment.
 * 
 * Hydra_Software_Devel/18   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/17   6/13/12 11:19a gprasad
 * SW7435-203: Increasing ATM to task internal fifo size to 8 per task to
 * fix the fifo overflow issue
 * 
 * Hydra_Software_Devel/16   6/6/12 12:59p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/15   2/8/12 8:25p ashoky
 * SW7346-598: Removing dynamic mem-alloc/free from decode start stop
 * time. Moving it to device  open/close.
 * 
 * Hydra_Software_Devel/14   2/3/12 11:54a ashoky
 * FWRAA-362: Raaga dsp sram optimization. Providing memory to swap
 * resident data.
 * 
 * Hydra_Software_Devel/13   1/27/12 1:32p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/12   9/30/11 3:01p ananthan
 * SW7429-20: Add 7429 support in basemodules/dsp
 * 
 * Hydra_Software_Devel/11   6/24/11 10:38p ashoky
 * SW7425-499: [7425] Adding to mainline
 * 
 * Hydra_Software_Devel/10   6/16/11 9:05p gprasad
 * SW7422-389: Add Dolby MS11 support - Adding suitable comments and using
 * cached copy to DRAM
 * 
 * Hydra_Software_Devel/9   6/14/11 10:05a gprasad
 * SW7422-389: Add Dolby MS11 support - Merging changes to main line
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/3   6/2/11 3:27p gprasad
 * SW7422-357: Need to add new FW mixer support for APE/DSP
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/2   6/2/11 3:22p gprasad
 * SW7422-357: Need to add new FW mixer support for APE/DSP
 * 
 * Hydra_Software_Devel/8   5/27/11 4:45p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/7   5/12/11 5:31p gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/MS11_DEVEL_BRANCH/1   5/11/11 11:13a gprasad
 * SW7422-389: [7422] Add Dolby MS11 support
 * 
 * Hydra_Software_Devel/6   5/4/11 4:26p gprasad
 * SW7422-367: Writeable global data in bdsp_cit_priv.c
 * 
 * Hydra_Software_Devel/5   4/14/11 10:56a gprasad
 * FWRAA-279: Phase 9p0 release for raaga
 * 
 * Hydra_Software_Devel/4   4/7/11 10:08a gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/3   4/6/11 12:18p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/3   4/6/11 12:18p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/3   4/6/11 12:17p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/3   4/6/11 12:16p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/2   4/6/11 11:39a gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/1   4/5/11 3:50p gprasad
 * SW7425-286: [7425] Checkin raaga binaries to magnum into basemodules
 * folder
 * 
 * Hydra_Software_Devel/5   3/5/11 4:42a srajapur
 * SW7422-316 : [7422] Merging Phase 1.0 release firmware code base to
 * BASE DSP code base
 * 
 * Hydra_Software_Devel/4   1/20/11 8:09a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/3   1/19/11 7:13a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/2   1/18/11 4:23a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/1   1/13/11 5:39a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/10   12/2/10 7:20p ashoky
 * SW7344-6: [7344] Adding CIT support for new raaga chips.
 * 
 * Hydra_Software_Devel/9   11/29/10 5:18p ashoky
 * SW35230-2087:[35230] Changing the I2S interface between CIT/PI. Stereo
 * and Multi I2S were interpreted differently between PI and CIT. Fixing
 * the same
 * 
 * Hydra_Software_Devel/8   11/15/10 3:10p ananthan
 * SW35125-15: [35125] Magnum RAP module needs to build for the 35125
 * 
 * Hydra_Software_Devel/7   10/13/10 8:57p ananthan
 * FWRAA-168, FWRAA-169, FWRAA-170: [35230] Adding support for custom DBE,
 * custom AVLP and custom ACF
 * 
 * Hydra_Software_Devel/6   10/13/10 8:18p ananthan
 * FWRAA-168, FWRAA-169, FWRAA-170: [35230] Adding support for custom DBE,
 * custom AVLP and custom ACF
 * 
 * Hydra_Software_Devel/5   8/3/10 7:15p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into main line
 * 
 * Hydra_Software_Devel/Raaga_Phase7p5tk2_Branch/1   7/26/10 8:30p gprasad
 * FWRAA-133: [35230] Merging all the host interface changes from phase
 * 6p0 to phase 7p5 take 2 into Raaga code base
 * 
 * Hydra_Software_Devel/3   6/15/10 8:01p ashoky
 * FW35230-33:[35230] Adding PLL programming part.
 * 
 * Hydra_Software_Devel/2   5/21/10 9:44p rpjamb
 * FW35230-32: [35230]Bring-up code for MPEG. CIT Changes to support
 * proper PLL programming by FW. This needs a modified brap_priv.h
 * 
 * Hydra_Software_Devel/1   4/30/10 1:10p dlimaye
 * FW35230-30: [35230] Checkin CIT files in base4 Move the CIT files from
 * label RAAGA_FW35230-29_20100419 in base3 folder to the base4 folder
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/5   3/30/10 11:20a dlimaye
 * FW35230-28: [35230] Checkin CIT files for BM
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/4   1/29/10 2:09p dlimaye
 * FWRAA-33: [35230] Make changes to comply with VMEM based function calls
 * CIT changes:
 * 1. Added a table containing VMEM address for every node
 * 2. This address is populated in the node cfg structure in the firmware
 * interface
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/3   11/26/09 2:57p dlimaye
 * FW35230-10: [35230] Add the functionality of Vom table download and
 * make emulation release
 * Modified the CIT interface file to support download of VOM table
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/2   10/6/09 11:37a dlimaye
 * FW35230-5:[35230] Firmware and CIT file checkin for 35230 RDB.
 * Compiled with 35230 RDB and changed PLL programming
 * 
 * Hydra_Software_Devel/RAAGA_EMULATION/1   9/11/09 1:46p dlimaye
 * FW35230-2:[3513]Check-in the initial Raaga CIT files + FW binaries to
 * magnum
 * Checking in CIT files in the branch RAAGA_EMULATION
 * 
 ***************************************************************************/

#ifndef _BRAAGA_CIT_PRIV_H__
#define _BRAAGA_CIT_PRIV_H__

#include "bdsp_raaga_fw.h"
#include "berr.h"
#include "bdsp_raaga_types.h"


/* Test Bench Emulation Defines*/
/*-----------------------------*/
#ifdef TESTBENCH_BUILD	

	typedef unsigned long  BDSP_CIT_P_Boolean;		/* MSVC Boolean is long */
	#define BDSP_CITGEN_P_False					((uint32_t)0)	/* MSVC False is 0 */
	#define BDSP_CITGEN_P_True					((uint32_t)1)	/* MSVC False is 1 */
	#define BDSP_MAX_PP_PER_BRANCH_SUPPORTED	((uint32_t)7)
	#define BDSP_MAX_STAGE_PER_BRANCH_SUPPORTED (BDSP_MAX_PP_PER_BRANCH_SUPPORTED+1)
	#define BDSP_MAX_PP_BRANCH_SUPPORTED		((uint32_t)3)
	#define BDBG_ENTER(x)						
	#define BDBG_LEAVE(x)						
	#define BDBG_ASSERT(x)	
	#define BSTD_UNUSED(x)				

#else
	typedef bool BDSP_CIT_P_Boolean;
	#define BDSP_CITGEN_P_False false
	#define BDSP_CITGEN_P_True	true

#endif

/* Need to check if this is required */
#define	CAP_PORT_SUPPORT
/*#define	BTSC_HACK_ENABLED*/

/***************************************/
/*	 CIT GENERATION CODE DEFINES	   */
/***************************************/
#define BDSP_CIT_P_ONLY_ONE_PLL_SUPPORT
#define BDSP_CIT_P_ENABLE_FORK_MATRIXING
#if 1
#define ANALYZE_IO_CFG 	
#endif
#define ENABLE_LOW_DELAY_API				/*Enabling Low delay code*/

/***************************************/
/*	 CIT GENERATION DEBUG CODE DEFINES */
/***************************************/

#define BDSP_CIT_P_CHECK_INPUT_RANGE
#define BDSP_CIT_P_CHECK_OUTPUT_RANGE
#define BDSP_CIT_P_PRINT_PPM_CFG

/***************************************/
/*		 Definition of some Macros	   */
/*			needed by PI ONLY 		   */
/***************************************/
/* PI needs this because they associate FW binaries with Algo IDs at the	   */
/* time of BDSP_Open even before call to CIT_gen. These indices should not be  */
/*	confused with NODE indices in network. These are just the starting of	   */
/*	the processing array index in AlgoExecInfo structure!!!					   */
/*******************************************************************************/
#define BDSP_CIT_P_FRAMESYNC_NODE_INFO_INDEX		0
#define BDSP_CIT_P_DECODE_NODE_INFO_INDEX			1
#define BDSP_CIT_P_PASSTHRU_NODE_INFO_INDEX			1
#define BDSP_CIT_P_ENCODE_NODE_INFO_INDEX			1
#define BDSP_CIT_P_POSTPROCESS_NODE_INFO_INDEX		1

/***************************************/
/*		 Definition of SIZEOF		   */
/***************************************/
#ifdef ZSIM_PLATFORM
	#define SIZEOF(x)	( sizeof(x) * 2 )
#else
	#define SIZEOF(x)	( sizeof(x) )
#endif

/***************************************/
/*		 Definition of MAX 		   */
/***************************************/
#define BDSP_CIT_P_MAX_VALUE(a,b)	(a>b) ? (a) : (b)


/***************************************/
/*	 FIRMWARE TASK CONFIGURATION	   */
/***************************************/

/*	This value should be same as that of BDSP_AF_P_MAX_IP_FORKS */
#define BDSP_P_MAX_FW_STG_INPUTS					((uint32_t)3)
#define BDSP_P_MAX_FW_STG_OUTPUTS					((uint32_t)6)
#define BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS			((uint32_t)5)	/*This is based on the maximum Port Type ever possible*/
													
#define BDSP_P_MAX_FW_STG_PER_FW_BRANCH				((uint32_t)(BDSP_RAAGA_MAX_STAGE_PER_BRANCH)) /* Max No of PP Stage + 1 Decode Stage *//*		((uint32_t)7)*/
#define BDSP_P_MAX_FW_BRANCH_PER_FW_TASK			((uint32_t)BDSP_RAAGA_MAX_BRANCH)
#define BDSP_P_MAX_IS_BUFFER_PER_FW_TASK			((uint32_t)BDSP_P_MAX_FW_BRANCH_PER_FW_TASK + 1)
#define BDSP_P_MAX_NODES_PER_FW_STG					((uint32_t)5)/*to be removed */
#define BDSP_P_MAX_NODES_IN_TASK					((uint32_t)20)/*to be removed */
#define BDSP_P_MAX_OUTPUTS							((uint32_t)8)
/*#define BDSP_P_MAX_INDEPENDENT_DELAY				((uint32_t)300)*/
#define BDSP_P_MAX_ALGOS_IN_TASK					((uint32_t)( BDSP_P_MAX_FW_BRANCH_PER_FW_TASK * \
																 BDSP_P_MAX_FW_STG_PER_FW_BRANCH ))

/* MEMORY REQUIRED FOR RAAGA SYSTEM SWAP */
#define	BDSP_P_FW_SYSTEM_SWAP_MEMORY_SIZE				((uint32_t)(12*1024))

/* STATIC MEMORY ALLOCATION FOR A TASK */

#define BDSP_CIT_P_TASK_SWAP_BUFFER_SIZE_INBYTES		((uint32_t)(2048*4)) /* Task Swap Buffer size in bytes */
#define BDSP_CIT_P_TASK_PORT_CONFIG_MEM_SIZE			((uint32_t)((((SIZEOF(BDSP_AF_P_sFMM_DEST_CFG)*BDSP_AF_P_MAX_NUM_PLLS + 20)+3)>>2)<<2))	/* Task's output port configuration memory size in bytes */
#define BDSP_CIT_P_TASK_SPDIF_USER_CFG_MEM_SIZE			((uint32_t)((((SIZEOF(BDSP_AF_P_sSPDIF_USER_CFG)*BDSP_AF_P_MAX_NUM_SPDIF_PORTS + 20) +3)>>2)<<2)) /* Task's SPDIF user configuration memory size in bytes for all ports*/
#define BDSP_CIT_P_TASK_FMM_GATE_OPEN_CONFIG			((uint32_t)((((SIZEOF(BDSP_AF_P_TASK_sFMM_GATE_OPEN_CONFIG) + 20)+3)>>2)<<2)) /* FMM gate open configuration memory size in bytes*/
#define BDSP_CIT_P_TASK_HW_FW_CONFIG					((uint32_t)((((SIZEOF(BDSP_AF_P_sFW_HW_CFG) + 20)+3)>>2)<<2)) 
#define BDSP_CIT_P_TASK_FS_MAPPING_LUT_SIZE				((uint32_t)((((SIZEOF(BDSP_AF_P_sOpSamplingFreq))+3)>>2)<<2)) 
#define	BDSP_CIT_P_TASK_STC_TRIG_CONFIG_SIZE			((uint32_t)((((SIZEOF(BDSP_AF_P_sStcTrigConfig))+3)>>2)<<2)) 


/*  This is the extra memory that the PI need to allocate 
	in the task memory location other than the memory 
	estimated from bdsp_af_priv.c file
*/
#define BDSP_CIT_P_TOTAL_TASK_CFG_MEMORY				((uint32_t)(BDSP_CIT_P_TASK_SWAP_BUFFER_SIZE_INBYTES + \
																	BDSP_CIT_P_TASK_PORT_CONFIG_MEM_SIZE	 + \
																	BDSP_CIT_P_TASK_FMM_GATE_OPEN_CONFIG	 + \
																	BDSP_CIT_P_TASK_HW_FW_CONFIG			 + \
																	BDSP_CIT_P_TASK_FS_MAPPING_LUT_SIZE		 + \
																	BDSP_CIT_P_TASK_STC_TRIG_CONFIG_SIZE ))
/* Threshold specific defines */
#define BDSP_CIT_P_MINIMUM_ALGO_THRESHOLD				((uint32_t)(6144))	/*This is based in the DDP HBR and AAC QSF followed by SRC case */
#define BDSP_CIT_P_MAXIMUM_RESIDUAL_COLLECTION			((uint32_t)(1024))	/*This value is based on the DSOLA*/

/*----------------------------*/
/* CIT Module Private Defines */
/*----------------------------*/
#define BDSP_CIT_P_INVALID							((uint32_t)(0x7fffffff))
#define BDSP_CIT_P_NODE0							((uint32_t)(0))
#define BDSP_CIT_P_SRC0								((uint32_t)(0))
#define BDSP_CIT_P_DST0								((uint32_t)(0))
#define BDSP_CIT_P_NUM_SPECIAL_NODES					((uint32_t)(1))
#define BDSP_CIT_P_NUM_SPECIAL_NODES_IN_SCM_CXT		((uint32_t)(0))
#define BDSP_CIT_P_TSM_NODE_INDEX					((uint32_t)(1))
#define BDSP_CIT_P_NUM_ALGO_NW_PASSES				((uint32_t)(2))
#define BDSP_CIT_P_INVALID_NODE_IDX					((uint32_t)(0xFFFFFFFF))
#define BDSP_CIT_P_PI_INVALID						((uint32_t)(-1))


#define BDSP_CIT_P_PRESENT							((uint32_t)(1))				
#define BDSP_CIT_P_ABSENT							((uint32_t)(0))				

#define BDSP_CIT_P_ASSOCIATED						((uint32_t)(1))				
#define BDSP_CIT_P_NOT_ASSOCIATED					((uint32_t)(0))				


/* Error Defines*/
/*--------------*/
/*	Need to define error codes for each module and each scenario */
#define BDSP_CIT_P_ERROR							((uint32_t)0xFFFFFFFF)
#define BDSP_AF_P_DRAM_ADDR_INVALID					((uint32_t)0x80000000)

/*---------------------------*/
/* Chip specific RDB defines */
/*---------------------------*/

#if((BCHP_CHIP == 3548)||(BCHP_CHIP == 3556))
	#define BDSP_3548_FAMILY				1
#else
	#define BDSP_3548_FAMILY				0
#endif

#if (BCHP_CHIP == 35230)|| (BCHP_CHIP == 35125)
	#define BCM_35230_FAMILY				1
#endif 

#if ((BDSP_3548_FAMILY == 1) || (BCM_35230_FAMILY == 1) || (BCHP_CHIP == 35233)|| (BCHP_CHIP == 35126))
	#define MAX_NUM_DACS_AVAILABLE			3
#elif ((BCHP_CHIP == 7420) || (BCHP_CHIP == 7422) || (BCHP_CHIP == 7425) || (BCHP_CHIP == 7429))
	#define MAX_NUM_DACS_AVAILABLE			2
#else 
	#define MAX_NUM_DACS_AVAILABLE			1
#endif 

#if(BDSP_3548_FAMILY == 1)
	
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO	
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI
	#define BDSP_CIT_AUD_FMM_PLL0_MACRO						BCHP_VCXO_CTL_MISC_AC0_MACRO
	#define BDSP_CIT_AUD_FMM_PLL1_MACRO						BCHP_VCXO_CTL_MISC_AC1_MACRO
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_0		(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1		(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0			(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0			(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1			(uint32_t)NULL

#elif (BCHP_CHIP == 35230)|| (BCHP_CHIP == 35125)

	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO_0	
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO1	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO_1
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI
	#define BDSP_CIT_AUD_FMM_PLL_MACRO						BCHP_VCXO_MISC_AUDIO_MODE_CTRL

	#define BDSP_CIT_AUD_FMM_PLL0_CTRL0						BCHP_AUDIO0_PLL_PLL_6CH_CTRL_0
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL0						BCHP_AUDIO1_PLL_PLL_6CH_CTRL_0
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL1						BCHP_AUDIO0_PLL_PLL_6CH_CTRL_1
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL1						BCHP_AUDIO1_PLL_PLL_6CH_CTRL_1
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL2						BCHP_AUDIO0_PLL_PLL_6CH_CTRL_2
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL2						BCHP_AUDIO1_PLL_PLL_6CH_CTRL_2
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL3						BCHP_AUDIO0_PLL_PLL_6CH_CTRL_3
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL3						BCHP_AUDIO1_PLL_PLL_6CH_CTRL_3
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL4						BCHP_AUDIO0_PLL_PLL_6CH_CTRL_4
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL4						BCHP_AUDIO1_PLL_PLL_6CH_CTRL_4
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL5						BCHP_AUDIO0_PLL_PLL_6CH_CTRL_5
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL5						BCHP_AUDIO1_PLL_PLL_6CH_CTRL_5
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL6						BCHP_AUDIO0_PLL_PLL_6CH_CTRL_6
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL6						BCHP_AUDIO1_PLL_PLL_6CH_CTRL_6
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT0					BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH0
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT0					BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH0
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT1					BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH1
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT1					BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH1
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT2					BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH2
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT2					BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH2
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT3					BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH3
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT3					BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH3
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT4					BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH4
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT4					BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH4
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT5					BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH5
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT5					BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH5


	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_0		(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1		(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0			(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0			BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1			BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1

#elif (BCHP_CHIP == 35233)|| (BCHP_CHIP == 35126)

	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO_0	
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO1	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO_1
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI
	#define BDSP_CIT_AUD_FMM_PLL_MACRO						BCHP_VCXO_MISC_AUDIO_MODE_CTRL

	#define BDSP_CIT_AUD_FMM_PLL0_CTRL0						BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_0
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL0						BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_0
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL1						BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_1
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL1						BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_1
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL2						BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_2
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL2						BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_2
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL3						BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_3
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL3						BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_3
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL4						BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_4
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL4						BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_4
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL5						BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_5
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL5						BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_5
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL6						BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_6
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL6						BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_6
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT0					BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_OUT_CH0
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT0					BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_OUT_CH0
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT1					BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_OUT_CH1
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT1					BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_OUT_CH1
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT2					BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_OUT_CH2
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT2					BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_OUT_CH2
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT3					BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_OUT_CH3
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT3					BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_OUT_CH3
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT4					BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_OUT_CH4
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT4					BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_OUT_CH4
	#define BDSP_CIT_AUD_FMM_PLL0_CTRL_OUT5					BCHP_PLL_AUDIO0_PLL_AUDIO0_DPLLGEN_CTRL_OUT_CH5
	#define BDSP_CIT_AUD_FMM_PLL1_CTRL_OUT5					BCHP_PLL_AUDIO1_PLL_AUDIO1_DPLLGEN_CTRL_OUT_CH5


	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_0		(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1		(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0			(uint32_t)NULL
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0			BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1			BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1

#else

	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0	
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO1	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO1
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0	BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0
	#define BDSP_CIT_AUD_FMM_PLL0_MACRO						BCHP_AUD_FMM_PLL0_MACRO
	#define BDSP_CIT_AUD_FMM_PLL1_MACRO						BCHP_AUD_FMM_PLL1_MACRO
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_0		BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_0
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1		BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1															
	#define BDSP_CIT_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0			BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0			BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0
	#define BDSP_CIT_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1			BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1
	
#endif
/*-----------------------------------------------------------------------*/


#ifdef TESTBENCH_BUILD

	/* These defines are kept for compilation is MSVC */
	#define BCHP_AUD_FMM_PLL0_MACRO							(uint32_t)NULL
	#define BCHP_AUD_FMM_PLL1_MACRO							(uint32_t)NULL
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO0		(uint32_t)NULL
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI0		(uint32_t)NULL
	#define BCHP_HIFIDAC_RM0_PHASE_INC						(uint32_t)NULL
	#define BCHP_HIFIDAC_RM0_RATE_RATIO						(uint32_t)NULL
	#define BCHP_HIFIDAC_RM0_SAMPLE_INC						(uint32_t)NULL
	#define BCHP_HIFIDAC_CTRL0_RATEMGRCFG					(uint32_t)NULL
	#define BCHP_HIFIDAC_CTRL1_RATEMGRCFG					(uint32_t)NULL
	#define BCHP_HIFIDAC_CTRL2_RATEMGRCFG					(uint32_t)NULL
	#define BCHP_HIFIDAC_RM1_PHASE_INC						(uint32_t)NULL
	#define BCHP_HIFIDAC_RM1_RATE_RATIO						(uint32_t)NULL
	#define BCHP_HIFIDAC_RM1_SAMPLE_INC						(uint32_t)NULL
	#define BCHP_HIFIDAC_RM2_PHASE_INC						(uint32_t)NULL
	#define BCHP_HIFIDAC_RM2_RATE_RATIO						(uint32_t)NULL
	#define BCHP_HIFIDAC_RM2_SAMPLE_INC						(uint32_t)NULL

	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_SPDIF0			(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_FW_CBIT_CTRL_0				(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_FW_CBITS0					(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_FW_CBITS12					(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_ESR_STATUS_CBIT_PING_PONG0_MASK	(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_FW_CBIT_CTRL_1				(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_FW_CBITS24					(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_FW_CBITS36					(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_ESR_STATUS_CBIT_PING_PONG1_MASK	(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_ESR_STATUS						(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_ESR_STATUS_CLEAR				(uint32_t)NULL
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_HBR0				(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_1			(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_HW_CHANSTAT_LO_0			(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_0				(uint32_t)NULL
	#define BCHP_AUD_FMM_MS_CTRL_HW_SPDIF_CFG_1				(uint32_t)NULL

	/* 3548  and 3556 Defines */
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO		(uint32_t)NULL
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_MULTI			(uint32_t)NULL
	#define BCHP_VCXO_CTL_MISC_AC0_MACRO					(uint32_t)NULL
	#define BCHP_VCXO_CTL_MISC_AC1_MACRO					(uint32_t)NULL	


	/* 7420 RDBS*/
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_MAI_MULTI			(uint32_t)NULL	
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO1		(uint32_t)NULL	

	/*7550 RDBs*/
	#define BCHP_AUD_FMM_OP_SPDIF_NCO_PHASE_INC				(uint32_t)NULL	
	#define BCHP_AUD_FMM_OP_SPDIF_NCO_RATE_RATIO			(uint32_t)NULL	
	#define BCHP_AUD_FMM_OP_SPDIF_NCO_SAMPLE_INC			(uint32_t)NULL	

	/*35230 RDBs*/
	#define BCHP_AUDIO0_PLL_PLL_DUAL_DIFF_IN_CTRL_0			(uint32_t)NULL
	#define BCHP_AUDIO0_PLL_PLL_DUAL_DIFF_IN_CTRL_1			(uint32_t)NULL
	#define BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_I2S_STEREO_0		(uint32_t)NULL

	#define BCHP_AUDIO0_PLL_PLL_6CH_CTRL_0					(uint32_t)NULL
	#define BCHP_AUDIO1_PLL_PLL_6CH_CTRL_0					(uint32_t)NULL
	#define BCHP_AUDIO0_PLL_PLL_6CH_CTRL_1					(uint32_t)NULL
	#define BCHP_AUDIO1_PLL_PLL_6CH_CTRL_1					(uint32_t)NULL
	#define BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH0			(uint32_t)NULL
	#define BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH0			(uint32_t)NULL
	#define BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH1			(uint32_t)NULL
	#define BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH1			(uint32_t)NULL
	#define BCHP_AUDIO0_PLL_PLL_6CH_CTRL_OUT_CH2			(uint32_t)NULL
	#define BCHP_AUDIO1_PLL_PLL_6CH_CTRL_OUT_CH2			(uint32_t)NULL

#else	

	#define BCHP_AUD_FMM_MS_CTRL_FW_CBITS0                (BCHP_AUD_FMM_MS_CTRL_FW_CBITSi_ARRAY_BASE)
	#define BCHP_AUD_FMM_MS_CTRL_FW_CBITS1                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS0  + 4)
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS2                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS1  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS3                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS2  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS4                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS3  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS5                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS4  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS6                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS5  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS7                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS6  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS8                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS7  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS9                (BCHP_AUD_FMM_MS_CTRL_FW_CBITS8  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS10               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS9  + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS11               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS10 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS12               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS11 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS13               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS12 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS14               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS13 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS15               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS14 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS16               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS15 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS17               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS16 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS18               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS17 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS19               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS18 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS20               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS19 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS21               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS20 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS22               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS21 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS23               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS22 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS24               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS23 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS25               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS24 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS26               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS25 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS27               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS26 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS28               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS27 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS29               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS28 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS30               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS29 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS31               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS30 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS32               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS31 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS33               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS32 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS34               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS33 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS35               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS34 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS36               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS35 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS37               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS36 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS38               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS37 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS39               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS38 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS40               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS39 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS41               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS40 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS42               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS41 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS43               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS42 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS44               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS43 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS45               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS44 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS46               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS45 + 4) 
    #define BCHP_AUD_FMM_MS_CTRL_FW_CBITS47               (BCHP_AUD_FMM_MS_CTRL_FW_CBITS46 + 4) 
#endif

typedef enum
{
	eFALSE = 0,
	eTRUE	
} BOOLEAN;

/***************************************************************************
Summary:
	This enumeration for Selecting Shorting input for Decode 1st node and
	Fs Node.

	For Audio, the input of FS and Decoder first node are the same. But this
	is not applicable for Video

***************************************************************************/
typedef enum BDSP_CIT_P_SameInputForFsAndDecode
{
	BDSP_CIT_P_SameInputForFsAndDecode_eDisable = 0x0,		/* For video */
	BDSP_CIT_P_SameInputForFsAndDecode_eEnable, 			/* For audio */
	BDSP_CIT_P_SameInputForFsAndDecode_eMax,
   	BDSP_CIT_P_SameInputForFsAndDecode_eInvalid	= 0x7FFFFFFF

}BDSP_CIT_P_SameInputForFsAndDecode;


/*********************************************************************
Summary:
    Enums that describes the FMM Destination type 

Description:
    This enum describes the various FMM Destination types :
			Output Port/Capture Port
	
See Also:
	BDSP_CIT_P_FmmDstSelect.
**********************************************************************/ 
typedef enum BDSP_CIT_P_FmmDstSelect
{

	BDSP_CIT_P_FmmDstSelect_eOpPort,			/*Output Port type*/
	BDSP_CIT_P_FmmDstSelect_eCapturePort,		/*Capture Port Type */	
	BDSP_CIT_P_FmmDstSelect_eMax,			    /* Max value */
   	BDSP_CIT_P_FmmDstSelect_eInvalid = 0x7FFFFFFF          

}BDSP_CIT_P_FmmDstSelect;

/*********************************************************************
Summary:
    Enums that describes the FMM port Type : Master or Slave

Description:
    This enum describes the various the FMM port Type: Master or Slave.
	Used for Master slave configuration of FMM port
	
See Also:
	BDSP_AudioEncode.
**********************************************************************/ 
typedef enum BDSP_CIT_P_MasterSlaveType
{
	BDSP_CIT_P_MasterSlaveType_eMasterPort,			/* Master Port*/
	BDSP_CIT_P_MasterSlaveType_eSlavePort,			/* Slave Port  */	
	BDSP_CIT_P_MasterSlaveType_eOthers,				/* Other Ports */	
 	BDSP_CIT_P_MasterSlaveType_eMax,			    /* Max value */
   	BDSP_CIT_P_MasterSlaveType_eInvalid	= 0x7FFFFFFF          
} BDSP_CIT_P_MasterSlaveType;



/***************************************************************************
Summary:
	This enumeration defines various processing modes.

***************************************************************************/
typedef enum BDSP_CIT_P_ProcessingMode
{
	BDSP_DSPCHN_ProcessingMode_ePP = 0x0,		/* Post proc */
	BDSP_DSPCHN_ProcessingMode_eMax, 			/* Max value */
   	BDSP_DSPCHN_ProcessingMode_eInvalid	= 0x7FFFFFFF
} BDSP_CIT_P_ProcessingMode;

/***************************************************************************
Summary:
	This enumeration defines various Video Decoder modes.

***************************************************************************/
typedef enum BDSP_CIT_P_VideoDecodeMode
{
	BDSP_CIT_P_VideoDecodeMode_eDecode = 0x0,	/* Decode */
	BDSP_CIT_P_VideoDecodeMode_eMax, 			/* Max value */
   	BDSP_CIT_P_VideoDecodeMode_eInvalid	= 0x7FFFFFFF
} BDSP_CIT_P_VideoDecodeMode;

/***************************************************************************
Summary:
	This enumeration for defining Master Task Detection Status	

***************************************************************************/
typedef enum BDSP_CIT_P_MixerTaskDetection
{
	BDSP_CIT_P_MixerTaskDetection_eNotDetected	= 0x0,	/* Not Detected  */
	BDSP_CIT_P_MixerTaskDetection_eDetected, 			/* Detected */
	BDSP_CIT_P_MixerTaskDetection_eMax,
   	BDSP_CIT_P_MixerTaskDetection_eInvalid	= 0x7FFFFFFF

}BDSP_CIT_P_MixerTaskDetection;

/*********************************************************************
Summary:
    Union to describe the Audio algorihtm running in the current stage.

Description:
    This union describes the Audio algorithm that is currently running
	in a stage of the algo network.

See Also:
**********************************************************************/ 
typedef union 
{
    BDSP_AudioType		eDecAudioAlgo;
    BDSP_AudioEncode		eEncAudioAlgo;
    BDSP_AudioProcessing   eProcAudioAlgo;
	BDSP_VideoType		eVideoAlgo; 
	BDSP_VideoEncode	eVideoEncAlgo;	
	BDSP_ScmType            eScmAlgo;
}BDSP_CIT_P_uAlgorithmType;

/*********************************************************************
Summary:
    Union to describe the Audio mode of the Algo running in the 
	current stage.

Description:
    This union describes the mode of the Audio algorithm that is 
	currently running in a stage of the algo network. for the decoder
	the mode could be decode or pass through mode. For an encoder, it
	could real time or Non real time mode. There is no specail mode
	for Post Proc Algos.

See Also:
**********************************************************************/ 
typedef union 
{
    BDSP_DecodeMode		eDecAudioMode;
	BDSP_EncodeMode		eEncAudioMode;
	BDSP_CIT_P_ProcessingMode	eProcessingAudioMode;
}BDSP_CIT_P_uAudioMode;

/*********************************************************************
Summary:
    Enum describing the type of the algo running on this stage.

Description:
    This enum describes the type of the algo running on this stage. This
	information is required to differentitate between decode, encode and
	post proc stages in the CIT generation module..

See Also:
**********************************************************************/ 
typedef enum BDSP_CIT_P_StageType
{
	BDSP_CIT_P_StageType_eDecode,
	BDSP_CIT_P_StageType_eEncode,
	BDSP_CIT_P_StageType_ePostProc,
	BDSP_CIT_P_StageType_eVideoDecode,	/* For Video on ZSP */
	BDSP_CIT_P_StageType_eVideoEncode,	/* For Video Encode */
	BDSP_CIT_P_StageType_eScm,
	BDSP_CIT_P_StageType_eMax,
	BDSP_CIT_P_StageType_eInvalid = 0x7FFFFFFF
} BDSP_CIT_P_StageType;


/*********************************************************************
Summary:
    Enumeration to describe type of source or destination for a FW stage

Description:
    This enumeration describes types of source/destiantion from/to 
	which a fw stage can receive/feed data.

See Also:
**********************************************************************/ 

typedef enum BDSP_CIT_P_FwStgSrcDstType
{
	BDSP_CIT_P_FwStgSrcDstType_eFwStg,			/* Source or destination is another FW stage */
	BDSP_CIT_P_FwStgSrcDstType_eRaveBuf,		/* Source or destination is an IO buffer */
	BDSP_CIT_P_FwStgSrcDstType_eFMMBuf,			/* Source or destination is an IO buffer */
	BDSP_CIT_P_FwStgSrcDstType_eRDB,	
	BDSP_CIT_P_FwStgSrcDstType_eDRAMBuf,		
	BDSP_CIT_P_FwStgSrcDstType_eInterTaskDRAMBuf,	/* The Dram Buffer Shared Across Multiple Tasks */
	BDSP_CIT_P_FwStgSrcDstType_eMax,
	BDSP_CIT_P_FwStgSrcDstType_eInvalid = 0x7FFFFFFF
} BDSP_CIT_P_FwStgSrcDstType;

/*********************************************************************
Summary:
    Structure to describe the Ring Buffer properties for Src or Dst 
	of a Stage.

Description:

See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_sIO_BUFF_ID
{
    uint32_t				ui32NumBuffers;				/*	Defines the number of 
															channels in the input 
															or output */
    BDSP_AF_P_BufferType    eBufferType;				/*	Defines the the location 
															of the input or output buffer. 
															This can take values defined 
															by eBUFFER_TYPE */

    uint32_t				ui32BufferId[BDSP_AF_P_MAX_CHANNELS]; 
														/* Valid for buffer types
                                                           BDSP_AF_P_eBufferType_RDB and
                                                           BDSP_AF_P_eBufferType_FMM */

	uint32_t				ui32AdaptRateCtrlIds[BDSP_AF_P_MAX_CHANNEL_PAIR]; /*Ids of Adapt rate ctrl block*/

}BDSP_CIT_P_sIO_BUFF_ID;

/*********************************************************************
Summary:
    Union to describe the IO Buffer properties for Src or Dst of a Stage.
Description:
    

See Also:
**********************************************************************/ 
/* Io Buffer*/
typedef union uIoBuff
{
    BDSP_AF_P_sIO_BUFFER			sIoBuffer;
    BDSP_CIT_P_sIO_BUFF_ID			sIoBuffId;

}uIoBuff;

/* Io generic Buffer*/
typedef union uIoGenereicBuff
{
	BDSP_AF_P_sIO_GENERIC_BUFFER	sIoGenericBuffer;
	BDSP_CIT_P_sIO_BUFF_ID			sIoGenericBuffId;

}uIoGenereicBuff;

/*Combined Structure*/
typedef struct BDSP_CIT_P_IoBuf
{
	uIoBuff				uIoBuffer;      /*	If source or destination is
											IO buffer, then details of it */

	uIoGenereicBuff		uIoGenericBuffer;
										/*	If source or destination is
											IO buffer, then details of it */

										/*	Independent delay if the
											buffer is output buffer */
	uint32_t			ui32IndepDelay ;	

	BDSP_AF_P_FmmDstFsRate	eFmmDstFsRate;		/* 
												   FMM Destination's associated sinking rate.
												   Like ui32IndepDelay, This parameter is associated only with OUTPUT FMM buffers .
												   Keep the value as Invalid for other cases.

												   Unlike ui32IndepDelay, eFmmDstFsRate will not change across Output ports in the
												   same Distinct output port.
												*/

} BDSP_CIT_P_IoBuf;

/*
	This structure is used to communicate the intertask DRAM buffer cfg.
	Here the DRAM Buffer will be allocated and configured in a master task
	and the handle will be passed to the slave task..

	Handle is passed from CIT(Master Task) -> PI and CIT (Slave Task).

	The slave task just passed the info to the DSP as the addr conversions
	are done in the Master Task.

*/
typedef struct BDSP_CIT_P_IoBufAddr       
{

	uint32_t	ui32IoBuffCfgAddr;			/* Io Buffer Config Address - This is an offset to the structure BDSP_AF_P_sIO_BUFFER*/
	uint32_t	ui32IoGenericBuffCfgAddr;	/* Io Generic Buffer Config Address - This is an offset to the structure BDSP_AF_P_sIO_GENERIC_BUFFER*/
	
}BDSP_CIT_P_IoBufAddr;

typedef struct BDSP_CIT_P_FwStage       
{
	uint32_t	ui32BranchId;
	uint32_t	ui32StageId;

	uint32_t	ui32DistinctOpNum;
	uint32_t	ui32OpNum;				/*	Id of the source stage output
											that is feeding data. 
											Valid only if FW stg is a source */
 
} BDSP_CIT_P_FwStage;					/*	If source or destination is
											another FW stage, then its
											branch id and stage id
										*/                                                             


/*********************************************************************
Summary:
    Structure for describing details of source or destination of a FW stage
Description:
    This structure describes details of a source or destiantion of a FW stage in a 
    network of stages of a firmware task.
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_FwStgSrcDstDetails
{
	BDSP_CIT_P_FwStgSrcDstType      eType;      /* Type of source or destination */
	
	union
	{
		BDSP_CIT_P_FwStage	sFwStage;			/*Stage Connectivity*/
		BDSP_CIT_P_IoBuf	sIoBuf;				/*Buffer Connectivity*/
		BDSP_CIT_P_IoBufAddr	sInterTaskDramBuffCfg; /* Intertask DRAM buff cfg*/

	} uDetails;

}BDSP_CIT_P_FwStgSrcDstDetails;

/*********************************************************************
Summary:
    Structure for describing details of source or destination of a FW stage
Description:
    This structure describes details of a source or destiantion of a FW stage. In 
    addition to the src/dst details it also provides the DRAM addresses alloocated 
    for the IO and IO  Generic buffer structures. Since the allocation for these
    structures will not be done in the CIT, it is upto the PI to allocated the memory 
    for these structures and provide the DRAM address for the same.
See Also:
**********************************************************************/ 
typedef struct BDSP_SIPS_P_FwStgSrcDstDetails
{
	BDSP_CIT_P_FwStgSrcDstType      eType;      /* Type of source or destination */
	
	union
	{
		BDSP_CIT_P_FwStage	sFwStage;			/*Stage Connectivity*/
		BDSP_CIT_P_IoBuf	sIoBuf;				/*Buffer Connectivity*/
	} uDetails;

	BDSP_CIT_P_IoBufAddr	sIoBuffAddrs; /* Intertask DRAM buff cfg*/

}BDSP_SIPS_P_FwStgSrcDstDetails;

/*********************************************************************
Summary:
    Structure for describing connectivity of a FW stage with other stages
Description:
    This structure describes connectivity of a FW stage with other stages in the FW
    task as well as with input/output buffers.
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_FwStgConnectivity
{
    uint32_t						ui32NumSrc; /*  Number of sources feeding data
													to this stage */

    BDSP_CIT_P_FwStgSrcDstDetails	sSrcDetails[BDSP_P_MAX_FW_STG_INPUTS];
												/*	Details of sources feeding data
													to this stage. Only first uiNumSrc
													number of entries of this array 
													are valid */

	uint32_t						ui32NumDistinctOp;
    uint32_t						ui32NumDst[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS];   
												/*	Number of destinations for each distinct o/p */

	BDSP_AF_P_DistinctOpType        eDistinctOpType[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS];
                                                /*	Op type for each distinct o/p */                                                                                                

    BDSP_CIT_P_FwStgSrcDstDetails   sDstDetails[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS][BDSP_P_MAX_FW_STG_OUTPUTS];
                                                /*	Details of destinations receiving data
													from distinct outputs of this stage. 
													Only uiNumDst number of entries 
													of this array are valid. The first
													dimension of this array represents
													distinct outputs and second dimension
													of array represents stages that receive
													data from the corresponding output in
													first dimension. 
												*/
} BDSP_CIT_P_FwStgConnectivity;

/*********************************************************************
Summary:
    Structure that contains firmware stage information
Description:
    This structure contains information about a firmware stage. It 
	contains information like position of firmware stage (branch id, 
	stage id), connectivity of the stage with other stages in the 
	firmware task and algorithm running in the stage.
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_FwStageInfo
{
    uint32_t					ui32BranchId;       /* Id of FW branch on which
													this stage is residing */

    uint32_t					ui32StageId;        /* Id of FW stage on FW
													branch */
	BDSP_CIT_P_StageType	eStageType;
    BDSP_CIT_P_uAlgorithmType	uAlgorithm;			/* Audio algorithm running
													in this stage */
	BDSP_CIT_P_uAudioMode		uAudioMode;

    BDSP_CIT_P_FwStgConnectivity	sStgConnectivity;/* Information on connectivity
													with other stages */

} BDSP_CIT_P_FwStageInfo;

/*********************************************************************
Summary:
    This structure contains information of a firmware branch

Description:
    This structure contains information of a firmware branch. It 
	also contains information of all the stages in a branch.

See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_FwBranchInfo
{
	uint32_t					ui32NumStages;

    BDSP_CIT_P_FwStageInfo      sFwStgInfo[BDSP_P_MAX_FW_STG_PER_FW_BRANCH];

} BDSP_CIT_P_FwBranchInfo;

/*********************************************************************
Summary:
    Memory information for CIT module
Description:
    This structure contains the memory information required by the CIT module.
    CIT module splits these memory chunks into various buffers. Shared memory
    chunk is split by CIT module into following buffers
    - Scratch Buffer
    - Interstage Buffers
    - Interstage Interface Buffers
    Task memory is split by CIT module into following buffers
    - Interframe Buffers
    - Configuration Parameter Buffers
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_MemInfo
{
    uint32_t            ui32SharedMemPhysAdr;   /*	Physical address of memory chunk
													that is split by CIT module into various
													buffers as required by FW task. This
													chunk is shared with other FW tasks
													running on same processor. */
    uint32_t            ui32SharedMemSize;      /*	Size of memory chunk
													that is split by CIT module into various
													buffers as required by FW task This
													chunk is shared between FW tasks
													running on same processor. */
    uint32_t            ui32TaskMemPhysAdr;     /*	Physical address of memory chunk
													that is split by CIT module into various
													buffers as required by FW task. This
													chunk is specific to a task. */
    uint32_t            ui32TaskMemSize;        /*	Size of memory chunk
													that is split by CIT module into various
													buffers as required by FW task. This
													chunk is specific to a task. */
	/* Below mentioned memories are only for CIT generation module.  They dont go to FW */
	void				*psCitDataStructure;
	void				*psFwBrnachInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK];
	void				*psCitPortInfo;
	void				*psCitTaskIsInfo;

} BDSP_CIT_P_MemInfo;


/*********************************************************************
Summary:
	SPDIF Port Configuration

Description:

    This structure describes the SPDIF port Configuration
	  
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_SpdifCfg
{
    BDSP_CIT_P_Boolean    bPcmOnSpdif;    /* True  = Data is Compressed,
                                             False = Data is PCM */

    BDSP_CIT_P_Boolean    bHbr;           /* True = HBR mode 
                                             False= Non HBR mode  */                                                                     

	BDSP_CIT_P_Boolean    bHwCbits;      /*  True when HwCbits cfg is required */

} BDSP_CIT_P_SpdifCfg;


/*********************************************************************
Summary:
	MAI port Configuration

Description:

    This structure defines the Mux selector for the MAI port
	  
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_MaiCfg
{
	BDSP_OutputPort		eMaiMuxSelector;/* The Mai Input Selector */
	BDSP_CIT_P_SpdifCfg sMaiContent;	/* The Mai Payload Format Cfg
										   PCM - Compresssed - HBR 
										*/	
}BDSP_CIT_P_MaiCfg;


/*********************************************************************
Summary:

	BDSP_CIT_P_uFmmDstPortType

Description:

	This is a union that describes the Dest Type specifics in detail.
	a) Output Port type specifics
	b) Cap port specifics

See Also:
**********************************************************************/ 

typedef union	BDSP_CIT_P_uFmmDstPortType
{

       BDSP_OutputPort			 eOpType;		/* The FMM output Port type */
	   BDSP_CapInputPort		 eCapPortType;	/* The FMM Capture Port type */

}BDSP_CIT_P_uFmmDstPortType;

/*********************************************************************
Summary:
	BDSP_CIT_P_sFmmDstDetails

Description:

	This is a structure to describe the Destination port select and
	its type in detail.

	eFmmDstSelect : Selects Output Port / Cap port
	uFmmDstPortType :  Decribes utput Port / Cap port

See Also:
**********************************************************************/ 

typedef struct	BDSP_CIT_P_sFmmDstDetails
{
	BDSP_CIT_P_FmmDstSelect      eFmmDstSelect;		/* FMM Destination select*/   
	BDSP_CIT_P_uFmmDstPortType	 uFmmDstPortType;	/* FMM port type */

}BDSP_CIT_P_sFmmDstDetails;


/*********************************************************************
Summary:
	Port Configuration

Description:

    This structure describes the port Configuration

	HifiDac cfg: Even if the DAC is not configured with PLL,
				 it is expected that, CIT -PI interface needs an association 
				 btw HIFIDAC and PLL. This is done for simplicity in design.

				 Default PLL index for HIFIDAC when no other ports are available
				 in 0.			
				 The PLL index of HifiDac should be the same as that of other 
				 ports in the other case.
				

See Also:
**********************************************************************/ 
typedef struct	BDSP_CIT_P_PortCfg
{

	   BDSP_AF_P_EnableDisable		ePortEnableDisableFlag;	/* Port Enable/Disable: Used for SPS */

	   BDSP_CIT_P_sFmmDstDetails	sFmmDstDetails;			/* FMM Destination Details */	   

															/*	This field will show whether the port is really enabled or not.
																CIT generation module doenot need to honour this flag as it can
																assume the ports are enabled at the channel open time.
															*/
	
	   BDSP_AF_P_PllNcoSelect		ePllNcoSelect;			/*	Flag to indicate whether
																port is associated with PLL or NCO */

       uint32_t						ui32PllIndex;			/*	Pll-index of a PLL associated a port */


	   BDSP_AF_P_FmmDstFsRate		eFmmDstFsRate;			/* FMM Destination's associated sinking rate */

       union
       {
            BDSP_CIT_P_SpdifCfg sSpdifCfg;
			BDSP_CIT_P_MaiCfg	sMaiCfg; 

       } uOpCfg;

}BDSP_CIT_P_PortCfg;


/*********************************************************************
Summary:
	RDB Access Configuration

Description:

    This structure describes RDB Access Configuration

See Also:
**********************************************************************/ 
typedef struct  BDSP_CIT_P_RdbCfg
{
	uint32_t	ui32RdbBaseOffset;

	uint32_t	ui32RdbDspOffset;

}BDSP_CIT_P_RdbCfg;

/*********************************************************************
Summary:
    Structure to describe one MIT Entry

Description:
    This structure describes one entry of Master Index Table.

See Also:
**********************************************************************/ 
typedef struct BDSP_Raaga_P_Mit_Entry
{
    uint32_t			ui32AlgoCodePtr;
    uint32_t			ui32Algoui32CodeSize;
    uint32_t			ui32AlgoDataPtr;
    uint32_t			ui32AlgoDataSize;
}BDSP_Raaga_P_Mit_Entry;

/*********************************************************************
Summary:
    Structure to describe Master Index Table
Description:
    This structure describes Master Index Table.
See Also:
**********************************************************************/ 
typedef struct BDSP_Raaga_P_Mit
{
    bool    isExecutablesDownloaded[BDSP_AF_P_AlgoId_eMax];          /*Used when OpentimeDownload  is true*/
    BDSP_Raaga_P_Mit_Entry	sAlgoDwnldDetail[BDSP_AF_P_AlgoId_eMax];
}BDSP_Raaga_P_Mit;


/*********************************************************************
Summary:
    Input structure to CIT module
Description:
    This structure is input to CIT module and contains all the 
	information required by CIT module to prepare CIT structure.
	It contains
    1. Branch and Stage Information of audio algorithms 
    2. Memory to be used 
    3. Master Index Table 
    4. Input and Output Buffers 
    5. Independent Delay 
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_InputInfo
{
	uint32_t				ui32NumBranches;
	BDSP_CIT_P_FwBranchInfo	*pBranchInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK];
											/* Information of FW branches in a FW
											   task */
	BDSP_Raaga_P_Mit				*pMit;				/* Master Index Table */
	BDSP_CIT_P_MemInfo		memInfo;           /* Memory information */
	
	uint32_t                ui32NumOutputs;		/* Number of output ports for this task.
	                                           Only these many first entries in sPortCfg[]
	                                           array are valid. 
											*/
	
											/* Port Configuration*/
	BDSP_CIT_P_PortCfg		sPortCfg[BDSP_P_MAX_OUTPUTS];
											
	
	BDSP_CIT_P_RdbCfg		sRdbCfg;			/* RDB Access Configuration*/
	
	BDSP_AF_P_EnableDisable	eIndepDelayEnableFlag;
											/*BDSP_AF_P_eEnable  =  Independent delay enabled
											  BDSP_AF_P_eDisable =  Independent delay is disabled
											*/
	
	BDSP_AF_P_TimeBaseType	eTimeBaseType;		/* Time base type for a task 45Khz or 27 Mhz (Direct TV)
											*/
	BDSP_AF_P_DolbyMsUsageMode eDolbyMsUsageMode;
													/* This flag is required to modify the Frequency Map table */
	BDSP_AF_P_sStcTrigConfig	sStcTrigConfig;
												/* This is required to get trigger info for STC in soft increment mode (NRT cases) */	
				
    BDSP_AF_P_sOpSamplingFreq   *pSampleRateMap; /* Pointer to the input -> output sample rate mapping table */
} BDSP_CIT_P_InputInfo;


/*********************************************************************
Summary:
    Structure to describe the details of an interframe buffer for 
	a FW node

Description:
    This structure describes the details of an interframe buffer
	created in CIT module for a FW node.

See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_FwBufInfo
{
    BDSP_AF_P_AlgoId	eFwExecId;
    uint32_t			ui32InterframeBufAdr;
    uint32_t			ui32InterframeBufSize;
						
	uint32_t			ui32UserParamBufAdr;
    uint32_t			ui32UserParamBufSize;


	uint32_t			ui32StatusBufAdr;
    uint32_t			ui32StatusBufSize;
    
} BDSP_CIT_P_FwBufInfo;

/*********************************************************************
Summary:
    Structure to describe stage level auxiliary information generated 
	by CIT module
Description:
    This structure describes auxiliary information generated by CIT 
	module at stage level.
See Also:
**********************************************************************/

/*	WARNING!!! changed the user config info to an array of structures. 
	Need to communicate this to the PI team */
typedef struct BDSP_CIT_P_OpStgInfo
{
    BDSP_CIT_P_uAlgorithmType	uAlgorithm;			/*	Audio algorithm running
														in this stage */
	BDSP_CIT_P_StageType	eStageType;

	BDSP_CIT_P_uAudioMode		uAudioMode;

    uint32_t					ui32TsmNodeIndex;
    uint32_t					ui32StartNodeIndex; 														
    uint32_t					ui32NumNodes;
    BDSP_CIT_P_FwBufInfo		sFwOpNodeInfo[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];
} BDSP_CIT_P_OpStgInfo;

/*********************************************************************
Summary:
    Structure to describe branch level auxiliary information generated 
	by CIT module
Description:
    This structure describes auxiliary information generated by CIT
	module at branch level.
See Also:
**********************************************************************/

typedef struct BDSP_CIT_P_OpBranchInfo
{
	uint32_t					ui32NumStages;
    BDSP_CIT_P_OpStgInfo		sCitStgInfo[BDSP_P_MAX_FW_STG_PER_FW_BRANCH];

} BDSP_CIT_P_OpBranchInfo;

/*********************************************************************
Summary:
    Structure to describe output of CIT module
Description:
    This structure describes output of CIT module. Output of CIT module contains
    following 
    1. Completely initialized CIT structure
    2. Auxiliary information required by other  RAP PI modules to initilalize
        buffers created in CIT module
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_Output
{
    BDSP_AF_P_sTASK_CONFIG		sCit;			/* Cit Structure */
	
	uint32_t					ui32NumBranches;

    BDSP_CIT_P_OpBranchInfo		sCitBranchInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK];

	BDSP_AF_P_sDRAM_BUFFER		sStackSwapBuff;	/* Stack Swap Buffer */
	
	BDSP_AF_P_sDRAM_BUFFER		sSpdifUserConfigAddr[BDSP_AF_P_MAX_NUM_SPDIF_PORTS];

} BDSP_CIT_P_Output;



/*********************************************************************
Private structures of the CIT generation module
*********************************************************************/

/*The define is for IO buffer configuration memory size and is for both input and output */
#define BDSP_CIT_P_SIZEOF_IOBUFFCFG	(	SIZEOF(BDSP_AF_P_sIO_BUFFER) * 2 +\
										SIZEOF(BDSP_AF_P_sIO_GENERIC_BUFFER) * 2 )


/*********************************************************************
Summary:
    Enums that describes Input availability

Description:    
	Input present or absent

See Also:
	
**********************************************************************/ 

typedef enum
{
	BDSP_CIT_P_IP_STATUS_eNotAvaialble	= 0x0,
	BDSP_CIT_P_IP_STATUS_eAvaialble		= 0x1,
	BDSP_CIT_P_IP_STATUS_eMax
} BDSP_CIT_P_IP_STATUS;



/*********************************************************************
Summary:
    Structure to describe RDB Configuration
Description:

    This structure describes RDB offset Configuration
    1. CIT input Rdb cfg
    2. Final RDB offset
        
See Also:
**********************************************************************/ 

typedef struct BDSP_CIT_P_RdbConfig
{
	uint32_t				ui32TaskRdbOffset;
	BDSP_CIT_P_RdbCfg		sRdbCfg;

}BDSP_CIT_P_RdbConfig;

/*********************************************************************
Summary:
    Structure to describes all atributes of a Node
Description:
    Provides all information for the node to execute
        
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_sNodeAttr
{
	BDSP_AF_P_AlgoId				eAlgoId;

	/*	This is required as the Decode task need to know for taking
		Residue samples in consideration 
	*/
	uint32_t						ui32BranchId;	

	uint32_t						ui32AudioAlgorithmType;

	uint32_t						ui32CodeVomAddress;

	uint32_t						ui32CodeDramAddress;
	uint32_t						ui32CodeSize;

	uint32_t						ui32RomTableDramAddress;
	uint32_t						ui32RomTableSize;

	uint32_t						ui32InterFrmBuffDramAddress;
	uint32_t						ui32InterFrmBuffSize;

	uint32_t						ui32FwStatusBuffDramAddress;
	uint32_t						ui32FwStatusBuffSize;

	uint32_t						ui32InterStgIoBuffSize;
	uint32_t						ui32InterStgGenericBuffSize;

	uint32_t						ui32ScratchBuffDramAddress;
	uint32_t						ui32ScratchBuffSize;

	uint32_t						ui32UsrCfgBuffDramAddress;
	uint32_t						ui32UsrCfgBuffSize;

	uint32_t						ui32MaxSizePerChan;
	uint32_t						ui32MaxNumChansSupported;
	
	BDSP_AF_P_InterFrameBuffType	eInterFrameBuffType;

	BDSP_AF_P_FwStatus				eFwStatusBuffType;

	/*The stage output configuration. Added to sort the Output ports */
	uint32_t						ui32NumDistinctOp;
    uint32_t						ui32NumPortsPerDistOp[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS];   
	
	uint32_t						ui32NumSrc;
	uint32_t						ui32NumDst;

	uint32_t						ui32ModifiedNumDst;/*After removing redundant Interstage DST outputs*/
							
	uint32_t						ui32NodeIpBuffCfgAddr[BDSP_P_MAX_FW_STG_INPUTS]; /* These are offsets to the structure BDSP_AF_P_sIO_BUFFER*/
	uint32_t						ui32NodeIpGenericDataBuffCfgAddr[BDSP_P_MAX_FW_STG_INPUTS]; /* These are offsets to the structure BDSP_AF_P_sIO_GENERIC_BUFFER */
										
	/* Valid/Invalid Flag for the Input ports */
	BDSP_AF_P_ValidInvalid			eNodeIpValidFlag[BDSP_AF_P_MAX_IP_FORKS];						

	uint32_t						ui32NodeOpBuffCfgAddr[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS][BDSP_P_MAX_FW_STG_OUTPUTS]; /* These are offsets to the structure BDSP_AF_P_sIO_BUFFER*/
	uint32_t						ui32NodeOpGenericDataBuffCfgAddr[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS][BDSP_P_MAX_FW_STG_OUTPUTS]; /* These are offsets to the structure BDSP_AF_P_sIO_GENERIC_BUFFER */

	/* The data type of the output buffers of a node*/
	BDSP_AF_P_DistinctOpType		eNodeOpBuffDataType[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS][BDSP_P_MAX_FW_STG_OUTPUTS];	

	/*Stage Output connectivity*/
	BDSP_CIT_P_FwStgSrcDstType		eConnectivityType[BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS][BDSP_P_MAX_FW_STG_OUTPUTS];	

} BDSP_CIT_P_sNodeAttr;


/*********************************************************************
Summary:
    Structure to describes the Network Structure of all stages and 
	Branches in a task.

Description:
    Gives the complete Network details for a task
        
See Also:
**********************************************************************/ 

/* Stage information */
typedef struct BDSP_CIT_P_StageInfo
{
    uint32_t						ui32BranchId;   /*	Id of FW branch on which
														this stage is residing */

    uint32_t						ui32StageId;    /*	Id of FW stage on FW
														branch */
	BDSP_CIT_P_StageType		eStageType;
    BDSP_CIT_P_uAlgorithmType		uAlgorithm;     /*	Audio algorithm running
														in this stage */
	BDSP_CIT_P_uAudioMode			uAudioMode;
    BDSP_CIT_P_FwStgConnectivity	sStgConnectivity;
													/*	Information on connectivity
														with other stages */

	int32_t							bIpsFromFwStage; 
													/*	Indicates that the inputs are 
														from f/w stage only. In this case,
														special nodes are absent */


	uint32_t						ui32NumNodes;

	BDSP_CIT_P_sNodeAttr			sNodeAttr[BDSP_AF_P_MAX_NUM_NODES_IN_ALGO];

	uint32_t						ui32InterStageBuffIdx;


} BDSP_CIT_P_StageInfo;

/*--------------------*/
/* Branch information */
/*--------------------*/
typedef struct BDSP_CIT_P_BranchInfo
{
	uint32_t						ui32NumStages;

    BDSP_CIT_P_StageInfo			sStgInfo[BDSP_P_MAX_FW_STG_PER_FW_BRANCH];

} BDSP_CIT_P_BranchInfo;


/*--------------------------*/
/* Task network information */
/*--------------------------*/
typedef struct BDSP_CIT_P_AlgoNwInfo
{
	uint32_t						ui32NumBranches;   

	BDSP_CIT_P_BranchInfo			*psBranchInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK];

} BDSP_CIT_P_AlgoNwInfo;


/*********************************************************************
Summary:
    This datastructure is the basic structure to describe the stage 
	execution
Description:
	1) BranchId
	2) StageId
        
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_sAlgoExecOrder
{
    uint32_t                ui32BranchId;
    uint32_t                ui32StageId;

} BDSP_CIT_P_sAlgoExecOrder;

/*********************************************************************
Summary:
    This datastructure gives the memory requirement details for a task
	execution
Description:
	Memory requirement for a task        
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_sTaskBuffInfo
{
    uint32_t                ui32TaskInterFrmMemSize;
    uint32_t                ui32TaskUsrCfgMemSize;
    uint32_t                ui32TaskIoBuffCfgStructMemSize;
    uint32_t                ui32TaskScratchMemSize;

    uint32_t                ui32BranchInterStgIoMemSize;
    uint32_t                ui32BranchInterStgGenericMemSize;

    uint32_t                ui32TaskInterStgIoMemSize;
    uint32_t                ui32TaskInterStgGenericMemSize;
	uint32_t				ui32NumInterStgBuffs;

	uint32_t				ui32MaxSizePerChannel;
	uint32_t				ui32MaxNumChannelsSupported;
	
	uint32_t                ui32TaskFwStatusBuffMemSize;
	uint32_t                ui32TaskStackMemSize;
	
	uint32_t                ui32TaskPortConfigMemSize;
	uint32_t                ui32TaskSPDIFConfigMemSize;
	
	uint32_t                ui32TaskGateOpenConfigMemSize;

	uint32_t                ui32TaskHwFwCfgMemSize;

	uint32_t				ui32SamplingFrequencyMapLutSize;

	uint32_t				ui32StcTrigConfigMemSize;


} BDSP_CIT_P_sTaskBuffInfo;



/*********************************************************************
Summary:
    This datastructure describes Io Buffer and IO genreic buffer 
	configuration
Description:
	1) Io Buffer and
	2) Io generic Buffer
        
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_sBuffCfg
{
	uint32_t						ui32IoBufferStructAddr; /* This is an offset to the structure BDSP_AF_P_sIO_BUFFER */
	BDSP_AF_P_sIO_BUFFER			sIoBuffer;

	uint32_t						ui32GenericBufferStructAddr; /* This is an offset to the structure BDSP_AF_P_sIO_GENERIC_BUFFER */
	BDSP_AF_P_sIO_GENERIC_BUFFER	sGenericBuffer;


	uint32_t						ui32PortAssociated;
	uint32_t						ui32StageAssociated;

} BDSP_CIT_P_sBuffCfg;


/*********************************************************************
Summary:
    This datastructure describes Interstage buffer configuration of a
	Task
Description:
	Task's Interstage Configuration	
        
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_TaskIsBuffCfg
{
	/*	This array holds the I/O buff cfg structures inter-stage buffers */
	BDSP_CIT_P_sBuffCfg	sIsBuffCfg[BDSP_P_MAX_IS_BUFFER_PER_FW_TASK];

} BDSP_CIT_P_TaskIsBuffCfg;


typedef struct BDSP_CIT_P_sOpPortInfo
{
	uint32_t						ui32NumopPorts;
	BDSP_AF_P_sOP_PORT_CFG			sOpPortCfg[BDSP_AF_P_MAX_OP_PORTS];

} BDSP_CIT_P_sOpPortInfo;


/*********************************************************************
Summary:
    This datastructure describes the SPDIF configuration for a PLL
Description:
	1) Spdif Enable and
	2) Contents of SPDIF
        
See Also:
**********************************************************************/

typedef struct BDSP_CIT_P_sSpdifInputCfg
{
	uint32_t			  ui32SpdifEnabled;

	BDSP_CIT_P_Boolean    bPcmOnSpdif;	

	BDSP_CIT_P_Boolean    bHbr;                 
	
	BDSP_CIT_P_Boolean    bHwCbits;

}BDSP_CIT_P_sSpdifInputCfg;


/*********************************************************************
Summary:
    This datastructure describes the MAI configuration for a PLL
Description:
	1) Mai port Enable
	2) Mai Payload content and Mux Type	
        
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_sMaiInputCfg
{
	uint32_t			ui32MaiEnabled;
	BDSP_CIT_P_MaiCfg	sMaiCfg; 

}BDSP_CIT_P_sMaiInputCfg;

/*********************************************************************
Summary:
    This datastructure describes the IOP port Control information.
	This datastructure is the top control structure for DAC/I2S/CAP
Description:
	1) Port Enable
	2) Port Frequency Rate	
        
See Also: BDSP_CIT_P_sSpdifPortInfo

**********************************************************************/
typedef struct BDSP_CIT_P_sPortControlInfo
{
	uint32_t				ui32PortEnabled;

	BDSP_AF_P_FmmDstFsRate	eFmmDstFsRate;	/* FMM Destination's associated
											   sinking rate
											*/
}BDSP_CIT_P_sPortControlInfo;

/*********************************************************************
Summary:
    This datastructure describes the IOP port Control information for
	SPDIF port
	.
	
Description:
	1) Port Enable
	2) Port Frequency Rate	
    3) SPDIF data configuration
    
See Also:BDSP_CIT_P_sMaiPortInfo
**********************************************************************/
typedef struct BDSP_CIT_P_sSpdifPortInfo
{
	BDSP_CIT_P_sPortControlInfo		sSpdifPortControlInfo;

	BDSP_CIT_P_sSpdifInputCfg		sSpdifInputCfg;

}BDSP_CIT_P_sSpdifPortInfo;

/*********************************************************************
Summary:
    This datastructure describes the IOP port Control information for
	MAI port
	.
	
Description:
	1) Port Enable
	2) Port Frequency Rate	
    3) MAI data configuration
    
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_sMaiPortInfo
{
	BDSP_CIT_P_sPortControlInfo		sMaiPortControlInfo;

	BDSP_CIT_P_MaiCfg				sMaiCfg; 

}BDSP_CIT_P_sMaiPortInfo;

/*********************************************************************
Summary:
    This datastructure is the database for the ports w.r.t PLL/NCO
Description:
	Mentions the ports associated with single PLL or NCO
        
See Also:
**********************************************************************/
typedef struct BDSP_CIT_P_sPllPortAssociation
{		
	BDSP_AF_P_PllNcoSelect			ePllNcoSelect;

	uint32_t						ui32PllEnable;	
	uint32_t						ui32PllIndex;		

	/* New Data structures */
	/*---------------------------------------------------------------------------------------*/

	BDSP_CIT_P_sPortControlInfo			sI2SPortInfo[BDSP_AF_P_MAX_NUM_I2S_PORTS];
	BDSP_CIT_P_sPortControlInfo			sDacPortInfo[BDSP_AF_P_MAX_NUM_DAC_PORTS];
	BDSP_CIT_P_sPortControlInfo			sCapPortInfo[BDSP_AF_P_MAX_NUM_CAP_PORTS];
	BDSP_CIT_P_sPortControlInfo			sDummyPortInfo[BDSP_AF_P_MAX_NUM_DUMMY_PORTS];

	BDSP_CIT_P_sSpdifPortInfo			sSpdifPortInfo[BDSP_AF_P_MAX_NUM_SPDIF_PORTS];		
	BDSP_CIT_P_sMaiPortInfo				sMaiPortInfo;
	/*---------------------------------------------------------------------------------------*/

#if 0
	uint32_t						ui32I2SEnabled[BDSP_AF_P_MAX_NUM_I2S_PORTS];
	uint32_t						ui32DacEnabled[BDSP_AF_P_MAX_NUM_DAC_PORTS];
	BDSP_CIT_P_sSpdifInputCfg		sSpdifInputCfg[BDSP_AF_P_MAX_NUM_SPDIF_PORTS];
	BDSP_CIT_P_sMaiInputCfg			sMaiInputCfg;

	uint32_t						ui32CapPortEnabled[BDSP_AF_P_MAX_NUM_CAP_PORTS];
	uint32_t						ui32DummyPortEnabled[BDSP_AF_P_MAX_NUM_DUMMY_PORTS];
#endif

} BDSP_CIT_P_sPllPortAssociation;


/*********************************************************************
Summary:
    The datastructure used for Port Configuration
Description:
	The datastructure used for Port Sorting and the port configuration
        
See Also:
**********************************************************************/

typedef struct
{
	BDSP_AF_P_sFMM_DEST_CFG				sFmmDestCfg[BDSP_AF_P_MAX_NUM_PLLS];	
	BDSP_CIT_P_sPllPortAssociation		sPllPortAssociation[BDSP_AF_P_MAX_NUM_PLLS];

}BDSP_CIT_P_sPortCfg;



/*********************************************************************
Summary:
    Structure for Algo present in the Node structure
	Optional fields for the CIT_Gen Structure in special case
Description:
	All the big datastructures of CIT Gen module is set here    
        
See Also:
**********************************************************************/ 
typedef struct
{
	uint32_t	ui32DDP_PassThruPresent;
	uint32_t	ui32DTS_EncoderPresent;
	uint32_t	ui32AC3_EncoderPresent;
	uint32_t	ui32DolbyPulsePresent;
	uint32_t	ui32DdrePresent;

}BDSP_CIT_P_sAlgoModePresent;

/*********************************************************************
Summary:
    Structure mentions special tasks in the system. Based on the special 
	task configuration, setting required to be modified from the conventional
	cases.

Description:	
        
See Also:
**********************************************************************/ 
typedef struct BDSP_CIT_P_sSpecialTaskPresent
{
	/* Mixer Task Detection*/
	BDSP_CIT_P_MixerTaskDetection			eMixerTaskDetectionFlag;

}BDSP_CIT_P_sSpecialTaskPresent;

/*********************************************************************/

typedef struct
{
	uint32_t	ui32IsRequired;
	uint32_t	ui32BranchAssociated;
	uint32_t	ui32InterstageBufferId;

}BDSP_CITGEN_P_sOutputIsCfg;


/*********************************************************************
Summary:
    Internal Datastructures for the function
							BDSP_CITGEN_P_AllocIoBuffCfg	

Description:	
        
See Also:
**********************************************************************/ 
typedef struct BDSP_CITGEN_P_sAllocIoBuffCfgStructures
{

	BDSP_AF_P_sIO_BUFFER			sIoBuffTemp;
	BDSP_AF_P_sIO_GENERIC_BUFFER	sIoGenericBuffTemp;


}BDSP_CITGEN_P_sAllocIoBuffCfgStructures;


/*********************************************************************
Summary:
    The complete datastructure definition for CIT Module
Description:
	All the big datastructures of CIT Gen module is set here    
        
See Also:
**********************************************************************/ 

typedef struct
{
	BDSP_CIT_P_RdbConfig					sRdbCfg;
	BDSP_CIT_P_AlgoNwInfo					sAlgoNwInfo;	
	BDSP_CIT_P_sOpPortInfo					sOpPortInfo;	
	BDSP_CIT_P_sTaskBuffInfo				sTaskBuffInfo;
	BDSP_CIT_P_sAlgoExecOrder				sAlgoExecOrder[BDSP_P_MAX_ALGOS_IN_TASK];
	BDSP_AF_P_TASK_sFMM_GATE_OPEN_CONFIG	sTaskFmmGateOpenConfig;
	BDSP_AF_P_sDRAM_BUFFER					sTaskSPDIFPortConfigAddr[BDSP_AF_P_MAX_NUM_SPDIF_PORTS];
	/* Special Algorithm Stage detection flags */
	BDSP_CIT_P_sAlgoModePresent				sAlgoModePresent;

	/* Special Task Detection flag */
	BDSP_CIT_P_sSpecialTaskPresent			sSpecialTaskPresent;	

	BDSP_CITGEN_P_sOutputIsCfg				sOutputIsCfg[BDSP_P_MAX_ALGOS_IN_TASK][BDSP_P_MAX_FW_STG_DISTINCT_OUTPUTS];
	BDSP_AF_P_sFW_HW_CFG					sFwHwCfg;
	
	BDSP_CITGEN_P_sAllocIoBuffCfgStructures sAllocIoBuffCfgStructures;

}BDSP_CIT_P_sCitDataStructure;


/*---------------------------------------------------*/
/* Prototype Definition for CIT Genreation functions */
/*---------------------------------------------------*/

uint32_t BDSP_P_GenCit(	BMEM_Handle					hHeap,	
						BREG_Handle					hRegister,
						BDSP_CIT_P_InputInfo		*psCitIp,
						BDSP_CIT_P_Output			*psCitOp
					  );


void BDSP_P_AnalyseCit(BMEM_Handle					hHeap, 
					   BDSP_AF_P_sTASK_CONFIG		*psCit
					  );



/*--------------------------------------------------------------------------*/
/* Prototype Definition  and Structures for Calc Threshold and Block Time Fn*/
/*--------------------------------------------------------------------------*/

/*********************************************************************
Summary:
    Enums that describes whether the low delay mode is enabled/disabled

Description:
    PI conveys this information to API to indicate Lowdelay is enabled
	or not

See Also:
	
**********************************************************************/ 

typedef enum BDSP_CIT_P_LowDelayEnableMode
{
	BDSP_CIT_P_LowDelayMode_eDisabled,
	BDSP_CIT_P_LowDelayMode_eEnabled	=1,
	BDSP_CIT_P_LowDelayMode_eMax,			        /* Max value */
   	BDSP_CIT_P_LowDelayMode_eInvalid	= 0x7FFFFFFF          

}BDSP_CIT_P_LowDelayEnableMode;


/*********************************************************************
Summary:
    This structure contain elements that is returned by 
	CalcThreshold_BlockTime_AudOffset API.

Description:

	ui32Threshold and ui32BlockTime goes to FW and 
	ui32AudOffset goes to Application and TSM user cfg
        
See Also:
**********************************************************************/ 
typedef struct BDSP_CTB_Output
{
	uint32_t ui32Threshold;					/* Interms of samples */
	uint32_t ui32BlockTime;					/* Interms of Time (msec)  */
	uint32_t ui32AudOffset;					/* AudOffset in Time (msec) */

}BDSP_CTB_Output;




void BDSP_CITGEN_P_CalcThresholdAndBlockTime(BDSP_CTB_Output *psCTB_OutputStructure
											);

void BDSP_CITGEN_P_CalcThresholdZeroFillTimeAudOffset(
							BDSP_CIT_P_LowDelayEnableMode	eLowDelayEnableMode,
							BDSP_CIT_P_InputInfo			*psCitIp,
							BDSP_CTB_Output					*psCTB_OutputStructure
						);

/*-----------------------------------------------------------------------------*/
/* Prototype Definition  and Structures for Seamless Input Port Switching API  */
/*-----------------------------------------------------------------------------*/

/*********************************************************************
Summary:		
		This enum is the command to SIPS API to perform Adding/removing 
		a port.

Description:		
        
See Also:
**********************************************************************/ 
typedef enum BDSP_SIPS_P_AddRemoveIpPort
{
	BDSP_SIPS_P_AddRemoveIpPort_eAddPort,
	BDSP_SIPS_P_AddRemoveIpPort_eRemovePort	=1,
	BDSP_SIPS_P_AddRemoveIpPort_eMax,			        /* Max value */
   	BDSP_SIPS_P_AddRemoveIpPort_eInvalid	= 0x7FFFFFFF          

}BDSP_SIPS_P_AddRemoveIpPort;

/*********************************************************************
Summary:		
		This enum describes whether a free Ip port found or not

Description:		
        
See Also:
**********************************************************************/ 
typedef enum BDSP_SIPS_P_FreeIpPortFound
{
	BDSP_SIPS_P_FreeIpPortFound_ePresent,
	BDSP_SIPS_P_FreeIpPortFound_eAbsent	=1,
	BDSP_SIPS_P_FreeIpPortFound_eMax,			        /* Max value */
   	BDS_SIPS_P_FreeIpPortFound_eInvalid	= 0x7FFFFFFF          

}BDSP_SIPS_P_FreeIpPortFound;

/*********************************************************************
Summary:		
		This structure is the input to the SIPS-API(Seamless Input Port
															 Switching)	
Description:
		
		a) AddRemoveIpPort Command
		b) Input Port IndexToRemove
		c) Cit Data Structure Addr
		d) Working Cit Data Structure Addr
        
See Also:
**********************************************************************/ 
typedef struct BDSP_SIPS_InputInfo
{
	/*Rdb Configuration */
	BDSP_CIT_P_RdbCfg		sRdbCfg;

	/* Command to add or remove the port from CIT input of a Task */
	BDSP_SIPS_P_AddRemoveIpPort		eAddRemoveIpPort;

	/* Input Source port details to be added */
	BDSP_SIPS_P_FwStgSrcDstDetails	sInputSrcDetails;

	/* Input Port index to remove from the system */
	uint32_t						ui32InputPortIndexToRemove;

	/* The DRAM base address of CIT Data structure of the Running task
	   which needed to be reconfigured.
	*/
	uint32_t						ui32CitDataStructureDramAddr;

	/*
		DRAM address of the Working buffer for CIT modification. 

		Size of the buffer will be of the size BDSP_AF_P_sTASK_CONFIG.

		API Copies the CIT datastruture from 'ui32CitDataStructureDramAddr'
		to 'ui32WorkingCitDataStructureDramAddr'and modifies the structure 
	    parameters.

	*/

	uint32_t						ui32WorkingCitDataStructureDramAddr;


}BDSP_SIPS_InputInfo;


/*********************************************************************
Summary:		
		This enum describes Status of SIPS API Execution

Description:		
        
See Also:
**********************************************************************/ 
typedef enum BDSP_SIPS_P_ExecutionStatus
{
	BDSP_SIPS_P_ExecutionStatus_eSuccess,
	BDSP_SIPS_P_ExecutionStatus_eFail	=1,
	BDSP_SIPS_P_ExecutionStatus_eMax,			        /* Max value */
   	BDSP_SIPS_P_ExecutionStatus_eInvalid	= 0x7FFFFFFF          

}BDSP_SIPS_P_ExecutionStatus;

/***********************************************************************
Summary:		
	This structure is the output from  the SIPS-API (Seamless Input Port 
															  Switching)
Description:
		
		a) AddRemoveIpPort Command
		b) Input Port IndexToRemove
		c) Cit Data Structure Addr
		d) Working Cit Data Structure Addr
        
See Also:
*************************************************************************/ 
typedef struct BDSP_SIPS_OutputInfo
{
	BDSP_SIPS_P_ExecutionStatus	eExecutionStatus;

	uint32_t					ui32InputPortIndexAdded;		

												/* 
													Valid only with while adding 
													an Input port.
												*/
}BDSP_SIPS_OutputInfo;

/************************************************************************/ 

/*		Function Prototype for Seamless Input Port Switching(SIPS) API  */
/*----------------------------------------------------------------------*/
uint32_t BDSP_SIPS_P_SeamlessIpPortSwitchAPI(	
				BMEM_Handle				hHeap,				
				BDSP_SIPS_InputInfo		*psSipsIp,
				BDSP_SIPS_OutputInfo	*psSipsOp						
			);


/*---------------------------------------------------------------------*/
/* Prototype Definition  and Structures for Seamless Port Switching API*/
/*---------------------------------------------------------------------*/

/*********************************************************************
Summary:		
		This structure is the input to the SPS-API(Seamless Port Switching)
		It gives the TaskID and the FMM cfg buffers.

Description:
		
		a) Task ID
		b) FMM cfg memory address of running task
		c) The temp :operating buffer for SPS-API
        
See Also:
**********************************************************************/ 

typedef struct BDSP_SPS_InputInfo
{
	/*Task ID of the Task for which SPS API(Seamless Port Switching) is
	called. 	*/
	uint32_t	ui32TaskId;	

	/*The FMM cfg memory addr that CIT allocated and FW using */
	uint32_t	ui32CitTaskFmmDestCfgMemoryAddr;

	/*	This is the memory allocated by PI for every task and and 
		SPS API use this buffer as operating buffer for port
		reconfiguration
	*/
	BDSP_AF_P_sDRAM_BUFFER		sOperatingTaskFmmDestCfgBuff;


}BDSP_SPS_InputInfo;

/*********************************************************************
Summary:
		This structure gives the info on the destination port to be 
		enabled or disabled in SPS API(Seamless Port Switching)....		
		
Description:
		
		Port type of Op Port and Cap Port is metioned.
		Enable/Disable Flag is selects future status of port.
        
See Also:
**********************************************************************/ 

typedef struct BDSP_SPS_InputPortCfg
{
	uint32_t                ui32NumOutputs;	/*	Number of output ports for this task.
												Only these many first entries in sPortCfg[]
												array are valid. 
											*/

	BDSP_CIT_P_PortCfg		sPortCfg[BDSP_P_MAX_OUTPUTS];

}BDSP_SPS_InputPortCfg;


/*		Function Prototype for Seamless Port Switching(SPS) API			*/
/*----------------------------------------------------------------------*/
uint32_t BDSP_SPS_P_SeamlessPortSwitch(	
				BMEM_Handle				hHeap,
				BREG_Handle				hRegister,
				BDSP_SPS_InputInfo		*psSpsIp,
				BDSP_SPS_InputPortCfg	*psInputPortCfg						
			);
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Prototype Definition for function to display DEC/PP/ENC type		    */
/*----------------------------------------------------------------------*/

const char*  BDSP_P_PrintDecEncPpTypeName(
				BDSP_AF_P_DecodeEncPPAlgoType  eDecodeEncPPAlgoType									 
			);

/*----------------------------------------------------------------------*/

#if 0

void BDSP_CITGEN_P_GenerateFwRdbAddrTable(
							BDSP_CIT_P_RdbCfg	*psRdbCfg,
							uint32_t			ui32FwRdbTableDramAddr
						);
#endif


/****************************************************************************/
/****************************************************************************/
/************************* VIDEO TASK  **************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************
Summary:

    Union to describe task configuration for video decoder and encoder.

Description:
    1. Global task configuration of DSP task

    2. Details of the nodes present in DSP task

See Also:

******************************************************************************/ 

typedef union BDSP_VF_P_uTASK_CONFIG
{
	BDSP_VF_P_sDEC_TASK_CONFIG  sVideoDecTaskConfig;
	BDSP_VF_P_sENC_TASK_CONFIG  sVideoEncTaskConfig;
	
} BDSP_VF_P_uTASK_CONFIG; 

/****************************************************************************
Summary:

    Structure to describe output of Video CIT module

Description:

    This structure describes output of Video CIT module. Output of Video CIT 
	module contains following 

    1. Completely initialized Video CIT structure

    2. Auxiliary information required by other  RAP PI modules to initilalize
        buffers created in Video CIT module

See Also:

******************************************************************************/ 

typedef struct BDSP_CIT_P_VideoCITOutput
{
    BDSP_VF_P_uTASK_CONFIG			uVideoCit;			/* Video Cit Structure */

	uint32_t					ui32NumBranches;

    BDSP_CIT_P_OpBranchInfo		sCitBranchInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK];

	BDSP_AF_P_sDRAM_BUFFER		sStackSwapBuff;	/* Stack Swap Buffer */
	
	BDSP_AF_P_sDRAM_BUFFER		sSpdifUserConfigAddr[BDSP_AF_P_MAX_NUM_SPDIF_PORTS];

} BDSP_CIT_P_VideoCITOutput;

/*---------------------------------------------------------*/
/* Prototype Definition for Video CIT Genreation functions */
/*---------------------------------------------------------*/

uint32_t BDSP_P_VideoGenCit(
						BMEM_Handle					hHeap,	
						BREG_Handle					hRegister,						
						BDSP_CIT_P_InputInfo		*psCitIp,
						void						*psVideoBuffCfg,
						BDSP_CIT_P_VideoCITOutput	*psCitOp,
						BDSP_CIT_P_StageType		eStageType
					  );


void BDSP_P_AnalyseVideoCit(
						BMEM_Handle					hHeap,
						BDSP_VF_P_uTASK_CONFIG		*puCit,
						BDSP_CIT_P_StageType		eStageType
					  );

typedef struct BDSP_CIT_P_ScmCITOutput
{
        BDSP_SCM_P_sTASK_CONFIG         sScmCit;                /* SCM Cit Structure */

        uint32_t                                        ui32NumBranches;

        BDSP_CIT_P_OpBranchInfo         sCitBranchInfo[BDSP_P_MAX_FW_BRANCH_PER_FW_TASK];

        BDSP_AF_P_sDRAM_BUFFER          sStackSwapBuff; /* Stack Swap Buffer */

}BDSP_CIT_P_ScmCITOutput;

/*---------------------------------------------------------------------
			Top level SCM CIT Generation Function
---------------------------------------------------------------------*/

uint32_t BDSP_P_GenScmCit(	BMEM_Handle					hHeap,
								BREG_Handle					hRegister,
								BDSP_CIT_P_InputInfo		*psCitIp,
								BDSP_CIT_P_ScmCITOutput		*psScmCitOp 
							);


void BDSP_P_AnalyseScmCit(	BMEM_Handle				hHeap,
					   				BDSP_SCM_P_sTASK_CONFIG	*psScmCit
					  	 		); 

#endif
