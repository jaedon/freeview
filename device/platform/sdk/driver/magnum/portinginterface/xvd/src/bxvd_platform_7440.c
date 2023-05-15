/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7440.c $
 * $brcm_Revision: Hydra_Software_Devel/37 $
 * $brcm_Date: 7/20/11 3:05p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7440.c $
 * 
 * Hydra_Software_Devel/37   7/20/11 3:05p davidp
 * SW7420-2001: Reorder header file includes.
 * 
 * Hydra_Software_Devel/36   10/16/09 6:25p davidp
 * SW7401-4332: Implement new AVD reset procedure.
 * 
 * Hydra_Software_Devel/35   10/17/08 5:34p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/34   8/5/08 3:34p nilesh
 * PR45322: Move BCHP_DECODE_RVC references to platform specific register
 * structures
 * 
 * Hydra_Software_Devel/33   6/17/08 2:28p nilesh
 * PR43585: 7440 Multi-Decode firmware now loads
 * 
 * Hydra_Software_Devel/32   6/12/08 4:47p nilesh
 * PR43585: Updated multi-decode XVD to compile with 7440
 * 
 * Hydra_Software_Devel/31   6/10/08 4:37p davidp
 * PR38955: Added uiBvnf_Intr2_3_AvdStatus to platform register structure
 * to send to AVD FW.
 * 
 * Hydra_Software_Devel/30   5/14/08 12:33p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/29   11/6/07 5:54p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/28   10/31/07 11:09a btosi
 * PR36545: added "CabacBinDepth" to s_stPlatformReg_7440_AVD0 and
 * s_stPlatformReg_7440_AVD1
 * 
 * Hydra_Software_Devel/27   9/26/07 11:53a nilesh
 * PR34052: Fixed typo
 * 
 * Hydra_Software_Devel/26   8/29/07 7:38p davidp
 * PR34052: Remove unused register addr pointers from BXVD_Handle, use
 * platform_info.stReg structure to reference registers
 * 
 * Hydra_Software_Devel/25   8/22/07 3:49p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/24   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   8/9/07 3:50p nilesh
 * PR29915: Updated memory tables according to AVD Mosaic Doc v2.11 (Aug
 * 9, 2007)
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/5   7/16/07 2:00p nilesh
 * PR29915: Added PicDataRdy Interrupt 1 definition
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/4   7/13/07 5:15p nilesh
 * PR29915: Updated memory tables based on v2.9 of Mosaic API doc
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   7/10/07 6:31p nilesh
 * PR29915: Updated platform register definitions to match struct
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   6/19/07 12:13p nilesh
 * PR29915: Merge from mainline
 * 
 * Hydra_Software_Devel/22   6/12/07 11:37a nilesh
 * PR31900: Merged makefile cleanup to mainline to support bsettop
 * B_CONFIG_IMAGE build option
 * 
 * Hydra_Software_Devel/PR31900/1   6/7/07 12:13p nilesh
 * PR31900: Moved firmware image to separate file
 * 
 * Hydra_Software_Devel/21   5/15/07 3:04p pblanco
 * PR28215: Fixed cut/paste error found by Narasimha.
 * 
 * Hydra_Software_Devel/20   5/15/07 1:20p pblanco
 * PR28215: Added support for variable stripe width and multiple.
 * 
 * Hydra_Software_Devel/19   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 * 
 * Hydra_Software_Devel/PR30249/1   5/10/07 1:56p nilesh
 * PR30249: Updated memory configuration table to account for larger
 * stripe height (64) on 7440
 * 
 * Hydra_Software_Devel/18   2/28/07 12:43p davidp
 * PR28176: Use hardware interrupts for MBOX and WATCHDOG interrupts on B0
 * 
 * Hydra_Software_Devel/17   2/27/07 7:44p davidp
 * PR28176: Update decoder L2 interrupt names to match latest RDB for A0
 * and B0.
 * 
 * Hydra_Software_Devel/16   2/2/07 2:04p nilesh
 * PR 27283: Fix new interrupts for AVD 1
 * 
 * Hydra_Software_Devel/15   2/2/07 1:24p gaurava
 * PR 27283: New interrupts for 7403/7405 and made them consistent with
 * 7401.
 * 
 * Hydra_Software_Devel/14   1/23/07 1:43p pblanco
 * PR27283: New UART initialization support.
 * 
 * Hydra_Software_Devel/13   11/17/06 4:51p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/1   11/14/06 3:55p nilesh
 * PR25868: Added pre-relocated image support
 * 
 * Hydra_Software_Devel/12   11/13/06 2:03p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/PR25365/2   11/10/06 2:16p nilesh
 * PR25365: Added explicit size to firmware image array so that
 * unsupported firmware entries are set to NULL implicitly
 * 
 * Hydra_Software_Devel/PR25365/1   11/10/06 1:27p nilesh
 * PR25365: Added BIMG support
 * 
 * Hydra_Software_Devel/11   10/16/06 1:52p nilesh
 * PR23113: Added support for AVD 1 Core interrupts
 * 
 * Hydra_Software_Devel/9   9/8/06 2:35p nilesh
 * PR23113: Initial support for dual decode
 * 
 * Hydra_Software_Devel/8   9/8/06 10:39a nilesh
 * PR23113: Update to use core rev e1
 * 
 * Hydra_Software_Devel/7   9/5/06 3:05p davidp
 * PR22867: Merge secure heap (Cabac) support
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/31/06 9:50a nilesh
 * PR22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 * 
 * Hydra_Software_Devel/5   8/21/06 3:22p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/4   8/21/06 2:04p nilesh
 * PR22673: Fix compilation error
 * 
 * Hydra_Software_Devel/3   8/21/06 1:53p nilesh
 * PR22673: Restructure FW Load mechanism to use standard core firmware
 * struct definition
 * 
 * Hydra_Software_Devel/2   8/18/06 2:48p nilesh
 * PR23113: Updated to support 7440 RDB register names and single decoder
 * 
 * Hydra_Software_Devel/1   8/17/06 12:04p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/PR22673/3   8/16/06 3:23p nilesh
 * PR22673: Fix cross-platform build error
 * 
 * Hydra_Software_Devel/PR22673/2   8/16/06 3:11p nilesh
 * PR22673: Fixed BDBG typo
 * 
 * Hydra_Software_Devel/PR22673/1   8/16/06 3:06p nilesh
 * PR23113: 7440 Platform Support
 *
 ***************************************************************************/

#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_intr.h"
#include "bxvd_reg.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"
/* to pickup BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE */
#include "bchp_reg_cabac2bins2_0.h"
/* to pickup BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE */
#include "bchp_reg_cabac2bins2_1.h"

#include "bchp_decode_cpuaux_0.h"
#include "bchp_decode_cpuaux_1.h"
#include "bchp_decode_ind_sdram_regs_0.h"
#include "bchp_decode_ind_sdram_regs_1.h"
#include "bchp_decode_sint_0.h"
#include "bchp_decode_sint_1.h"
#include "bchp_decode_sint_oloop_0.h"
#include "bchp_decode_sint_oloop_1.h"
#include "bchp_decode_main_0.h"
#include "bchp_decode_main_1.h"

#include "bchp_decode_rvc_0.h"
#include "bchp_decode_rvc_1.h"

BDBG_MODULE(BXVD_PLATFORM_7440);

#ifdef BXVD_P_USE_INIT_REG_PTRS_7440A0

#if (BCHP_VER == BCHP_VER_A0)

static const BXVD_P_PlatformReg_RevE0 s_stPlatformReg_7440_AVD0 = 
{
	BCHP_SUN_TOP_CTRL_SW_RESET,                    /* SWReset */
	BCHP_SUN_TOP_CTRL_SW_RESET_avd0_sw_reset_MASK, /* SWResetAVDMask */	

	BCHP_AVD_INTR2_0_CPU_SET,                      /* CPUL2InterruptSet */
	BCHP_AVD_INTR2_0_CPU_CLEAR,                    /* CPUL2InterruptClear */
	BCHP_AVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */ 

        BCHP_BVNF_INTR2_3_AVD0_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
        BCHP_BVNF_INTR2_3_AVD0_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
        BCHP_BVNF_INTR2_3_AVD0_MASK_CLEAR ,            /* Bvnf_Intr2_3_AvdMaskClear */

        BCHP_XPT_PCROFFSET0_STC,                       /* XPT_PCROffset_STC */

	BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR10,  /* PicDataRdy */
        BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR13,  /* PicDataRdy1*/
	BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR11,  /* Mailbox */
	BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR14,  /* SeqHeader */
	BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR12,  /* StillPictureRdy */
	BCHP_INT_ID_AVD_INTR2_0_AVD_MBOX_INTR,  /* OuterWatchdog */
	BCHP_INT_ID_AVD_INTR2_0_VICH_REG_INTR,     /* VIChkrReg */
	BCHP_INT_ID_AVD_INTR2_0_VICH_SCB_WR_INTR,  /* VIChkrSCBWr */
	BCHP_INT_ID_AVD_INTR2_0_VICH_INST_RD_INTR, /* VIChkrInstrRead */
	
	BCHP_DECODE_CPUREGS_0_REG_INST_BASE,           /* InnerInstructionBase */
	BCHP_DECODE_CPUREGS_0_REG_END_OF_CODE,         /* InnerEndOfCode */
	BCHP_DECODE_CPUREGS_0_REG_GLOBAL_IO_BASE,      /* InnerGlobalIOBase */
        BCHP_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG,      /* InnerCPUDebug */
        BCHP_DECODE_CPUAUX_0_CPUAUX_REG,               /* InnerCPUAux */

	BCHP_DECODE_CPUREGS2_0_REG_INST_BASE,          /* OuterEndOfCode */
	BCHP_DECODE_CPUREGS2_0_REG_END_OF_CODE,        /* OuterInstructionBase */
	BCHP_DECODE_CPUREGS2_0_REG_GLOBAL_IO_BASE,     /* OuterGlobalIOBase */
	BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,     /* OuterCPUDebug */
	BCHP_DECODE_CPUAUX2_0_CPUAUX_REG,              /* OuterCPUAux */
	BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_MASK,    /* OuterInterruptMask */
	BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_CLR,     /* OuterInterruptClear */
	BCHP_DECODE_CPUREGS2_0_REG_WATCHDOG_TMR_LIMIT, /* OuterWatchdogTimerLimit */
	BCHP_DECODE_CPUREGS2_0_REG_WATCHDOG_TMR,       /* OuterWatchdogTimer */
	BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_MBX,        /* OuterCPU2HostMailbox */
	BCHP_DECODE_CPUREGS2_0_REG_HST2CPU_MBX,        /* OuterHost2CPUMailbox */
        BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_STAT,       /* OuterCPU2HostStatus */

	BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,   /* CabacBinDepth */	

        BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
        BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
        BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
        BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
        BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
        BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
        BCHP_DECODE_MAIN_0_REG_MAINCTL,                       /* uiDecode_MainCtl */
        BCHP_DECODE_MAIN_0_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */

        BCHP_SUN_GISB_ARB_REQ_MASK,                    /* SunGisbArb_ReqMask */
        BCHP_SUN_GISB_ARB_REQ_MASK_req_mask_7_MASK,          /* SunGisbArb_ReqMaskAVDMask */

        BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
        BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
        BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
        BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
        BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */
};

static const BXVD_P_PlatformReg_RevE0 s_stPlatformReg_7440_AVD1 = 
{
	BCHP_SUN_TOP_CTRL_SW_RESET,                    /* SWReset */
	BCHP_SUN_TOP_CTRL_SW_RESET_avd1_sw_reset_MASK, /* SWResetAVDMask */	

	BCHP_AVD_INTR2_1_CPU_SET,                      /* CPUL2InterruptSet */
	BCHP_AVD_INTR2_1_CPU_CLEAR,                    /* CPUL2InterruptClear */
	BCHP_AVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

        BCHP_BVNF_INTR2_3_AVD1_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
        BCHP_BVNF_INTR2_3_AVD1_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
        BCHP_BVNF_INTR2_3_AVD1_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */

        BCHP_XPT_PCROFFSET1_STC,                       /* XPT_PCROffset_STC */

	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR10, /* PicDataRdy */
        BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR13, /* PicDataRdy1 */
	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR11, /* Mailbox */
	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR14, /* SeqHeader */
	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR12, /* StillPictureRdy */
	BCHP_INT_ID_AVD_INTR2_1_AVD_MBOX_INTR, /* OuterWatchdog */
	BCHP_INT_ID_AVD_INTR2_1_VICH_REG_INTR,     /* VIChkrReg */
	BCHP_INT_ID_AVD_INTR2_1_VICH_SCB_WR_INTR,  /* VIChkrSCBWr */
	BCHP_INT_ID_AVD_INTR2_1_VICH_INST_RD_INTR, /* VIChkrInstrRead */	

	BCHP_DECODE_CPUREGS_1_REG_INST_BASE,           /* InnerInstructionBase */
	BCHP_DECODE_CPUREGS_1_REG_END_OF_CODE,         /* InnerEndOfCode */
	BCHP_DECODE_CPUREGS_1_REG_GLOBAL_IO_BASE,      /* InnerGlobalIOBase */
        BCHP_DECODE_IND_SDRAM_REGS_1_REG_CPU_DBG,      /* InnerCPUDebug */
        BCHP_DECODE_CPUAUX_1_CPUAUX_REG,               /* InnerCPUAux */

	BCHP_DECODE_CPUREGS2_1_REG_INST_BASE,          /* OuterEndOfCode */
	BCHP_DECODE_CPUREGS2_1_REG_END_OF_CODE,        /* OuterInstructionBase */
	BCHP_DECODE_CPUREGS2_1_REG_GLOBAL_IO_BASE,     /* OuterGlobalIOBase */
	BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG,     /* OuterCPUDebug */
	BCHP_DECODE_CPUAUX2_1_CPUAUX_REG,              /* OuterCPUAux */
	BCHP_DECODE_CPUREGS2_1_REG_CPU_INTGEN_MASK,    /* OuterInterruptMask */
	BCHP_DECODE_CPUREGS2_1_REG_CPU_INTGEN_CLR,     /* OuterInterruptClear */
	BCHP_DECODE_CPUREGS2_1_REG_WATCHDOG_TMR_LIMIT, /* OuterWatchdogTimerLimit */
	BCHP_DECODE_CPUREGS2_1_REG_WATCHDOG_TMR,       /* OuterWatchdogTimer */
	BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_MBX,        /* OuterCPU2HostMailbox */
	BCHP_DECODE_CPUREGS2_1_REG_HST2CPU_MBX,        /* OuterHost2CPUMailbox */
        BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_STAT,       /* OuterCPU2HostStatus */

	BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,   /* CabacBinDepth */	

        BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
        BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
        BCHP_DECODE_SINT_1_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
        BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
        BCHP_DECODE_SINT_OLOOP_1_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
        BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
        BCHP_DECODE_MAIN_1_REG_MAINCTL,                       /* uiDecode_MainCtl */
        BCHP_DECODE_MAIN_0_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */

        BCHP_SUN_GISB_ARB_REQ_MASK,                    /* SunGisbArb_ReqMask */
        BCHP_SUN_GISB_ARB_REQ_MASK_req_mask_8_MASK,          /* SunGisbArb_ReqMaskAVDMask */

        BCHP_DECODE_RVC_1_REG_RVC_CTL,   /* RVC Control */
        BCHP_DECODE_RVC_1_REG_RVC_PUT,   /* RVC Put */
        BCHP_DECODE_RVC_1_REG_RVC_GET,   /* RVC Get */
        BCHP_DECODE_RVC_1_REG_RVC_BASE,  /* RVC Base */
        BCHP_DECODE_RVC_1_REG_RVC_END,   /* RVC End */
};

#else

static const BXVD_P_PlatformReg_RevE0 s_stPlatformReg_7440_AVD0 = 
{
	BCHP_SUN_TOP_CTRL_SW_RESET,                    /* SWReset */
	BCHP_SUN_TOP_CTRL_SW_RESET_avd0_sw_reset_MASK, /* SWResetAVDMask */	

	BCHP_AVD_INTR2_0_CPU_SET,                      /* CPUL2InterruptSet */
	BCHP_AVD_INTR2_0_CPU_CLEAR,                    /* CPUL2InterruptClear */
	BCHP_AVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */ 

        BCHP_BVNF_INTR2_3_AVD0_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
        BCHP_BVNF_INTR2_3_AVD0_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
        BCHP_BVNF_INTR2_3_AVD0_MASK_CLEAR ,            /* Bvnf_Intr2_3_AvdMaskClear */

        BCHP_XPT_PCROFFSET0_STC,                       /* XPT_PCROffset_STC */

	BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR0,      /* PicDataRdy */
        BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR3,      /* PicDataRdy1 */
	BCHP_INT_ID_AVD_INTR2_0_AVD_MBOX_INTR,     /* Mailbox */  
	BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR4,      /* SeqHeader */
	BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR2,      /* StillPictureRdy */
	BCHP_INT_ID_AVD_INTR2_0_AVD_WATCHDOG_INTR, /* OuterWatchdog */
	BCHP_INT_ID_AVD_INTR2_0_VICH_REG_INTR,     /* VIChkrReg */
	BCHP_INT_ID_AVD_INTR2_0_VICH_SCB_WR_INTR,  /* VIChkrSCBWr */
	BCHP_INT_ID_AVD_INTR2_0_VICH_INST_RD_INTR, /* VIChkrInstrRead */
	
	BCHP_DECODE_CPUREGS_0_REG_INST_BASE,           /* InnerInstructionBase */
	BCHP_DECODE_CPUREGS_0_REG_END_OF_CODE,         /* InnerEndOfCode */
	BCHP_DECODE_CPUREGS_0_REG_GLOBAL_IO_BASE,      /* InnerGlobalIOBase */

	BCHP_DECODE_CPUREGS2_0_REG_INST_BASE,          /* OuterEndOfCode */
	BCHP_DECODE_CPUREGS2_0_REG_END_OF_CODE,        /* OuterInstructionBase */
	BCHP_DECODE_CPUREGS2_0_REG_GLOBAL_IO_BASE,     /* OuterGlobalIOBase */
	BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,     /* OuterCPUDebug */
	BCHP_DECODE_CPUAUX2_0_CPUAUX_REG,              /* OuterCPUAux */
	BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_MASK,    /* OuterInterruptMask */
	BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_CLR,     /* OuterInterruptClear */
	(uint32_t) NULL,                               /* OuterWatchdogTimerLimit */
	BCHP_DECODE_CPUREGS2_0_REG_WATCHDOG_TMR,       /* OuterWatchdogTimer */
	BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_MBX,        /* OuterCPU2HostMailbox */
	BCHP_DECODE_CPUREGS2_0_REG_HST2CPU_MBX,        /* OuterHost2CPUMailbox */
        BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_STAT,       /* OuterCPU2HostStatus */

	BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */	

        BCHP_SUN_GISB_ARB_REQ_MASK,                    /* SunGisbArb_ReqMask */
        BCHP_SUN_GISB_ARB_REQ_MASK_req_mask_7_MASK,    /* SunGisbArb_ReqMaskAVDMask */

        BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
        BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
        BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
        BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
        BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */
};

static const BXVD_P_PlatformReg_RevE0 s_stPlatformReg_7440_AVD1 = 
{
	BCHP_SUN_TOP_CTRL_SW_RESET,                    /* SWReset */
	BCHP_SUN_TOP_CTRL_SW_RESET_avd1_sw_reset_MASK, /* SWResetAVDMask */	

	BCHP_AVD_INTR2_1_CPU_SET,                      /* CPUL2InterruptSet */
	BCHP_AVD_INTR2_1_CPU_CLEAR,                    /* CPUL2InterruptClear */
	BCHP_AVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

        BCHP_BVNF_INTR2_3_AVD1_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
        BCHP_BVNF_INTR2_3_AVD1_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
        BCHP_BVNF_INTR2_3_AVD1_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */

        BCHP_XPT_PCROFFSET1_STC,                       /* XPT_PCROffset_STC */

	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR0,      /* PicDataRdy */
	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR3,      /* PicDataRdy1 */
	BCHP_INT_ID_AVD_INTR2_1_AVD_MBOX_INTR,     /* Mailbox */
	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR4,      /* SeqHeader */
	BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR2,      /* StillPictureRdy */
	BCHP_INT_ID_AVD_INTR2_1_AVD_WATCHDOG_INTR, /* OuterWatchdog */
	BCHP_INT_ID_AVD_INTR2_1_VICH_REG_INTR,     /* VIChkrReg */
	BCHP_INT_ID_AVD_INTR2_1_VICH_SCB_WR_INTR,  /* VIChkrSCBWr */
	BCHP_INT_ID_AVD_INTR2_1_VICH_INST_RD_INTR, /* VIChkrInstrRead */
	
	BCHP_DECODE_CPUREGS_1_REG_INST_BASE,           /* InnerInstructionBase */
	BCHP_DECODE_CPUREGS_1_REG_END_OF_CODE,         /* InnerEndOfCode */
	BCHP_DECODE_CPUREGS_1_REG_GLOBAL_IO_BASE,      /* InnerGlobalIOBase */

	BCHP_DECODE_CPUREGS2_1_REG_INST_BASE,          /* OuterEndOfCode */
	BCHP_DECODE_CPUREGS2_1_REG_END_OF_CODE,        /* OuterInstructionBase */
	BCHP_DECODE_CPUREGS2_1_REG_GLOBAL_IO_BASE,     /* OuterGlobalIOBase */
	BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG,     /* OuterCPUDebug */
	BCHP_DECODE_CPUAUX2_1_CPUAUX_REG,              /* OuterCPUAux */
	BCHP_DECODE_CPUREGS2_1_REG_CPU_INTGEN_MASK,    /* OuterInterruptMask */
	BCHP_DECODE_CPUREGS2_1_REG_CPU_INTGEN_CLR,     /* OuterInterruptClear */
	(uint32_t) NULL,                               /* OuterWatchdogTimerLimit */
	BCHP_DECODE_CPUREGS2_1_REG_WATCHDOG_TMR,       /* OuterWatchdogTimer */
	BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_MBX,        /* OuterCPU2HostMailbox */
	BCHP_DECODE_CPUREGS2_1_REG_HST2CPU_MBX,        /* OuterHost2CPUMailbox */
        BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_STAT,       /* OuterCPU2HostStatus */

	BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */	

        BCHP_SUN_GISB_ARB_REQ_MASK,                    /* SunGisbArb_ReqMask */
        BCHP_SUN_GISB_ARB_REQ_MASK_req_mask_8_MASK,    /* SunGisbArb_ReqMaskAVDMask */

        BCHP_DECODE_RVC_1_REG_RVC_CTL,   /* RVC Control */
        BCHP_DECODE_RVC_1_REG_RVC_PUT,   /* RVC Put */
        BCHP_DECODE_RVC_1_REG_RVC_GET,   /* RVC Get */
        BCHP_DECODE_RVC_1_REG_RVC_BASE,  /* RVC Base */
        BCHP_DECODE_RVC_1_REG_RVC_END,   /* RVC End */
};
#endif

void BXVD_P_InitRegPtrs_7440A0(BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_InitRegPtrs_7440A0);

   if (hXvd->uDecoderInstance == 0) 
   {
      /* Platform Info */
      hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID = BXVD_IMAGE_FirmwareID_eOuterELF_AVD0;
      hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = BXVD_IMAGE_FirmwareID_eInnerELF_AVD0;
      hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID = BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0;
	   
      hXvd->stPlatformInfo.stReg = s_stPlatformReg_7440_AVD0;
   } 
   else
   {
      /* Platform Info */
      hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID = BXVD_IMAGE_FirmwareID_eOuterELF_AVD1;
      hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = BXVD_IMAGE_FirmwareID_eInnerELF_AVD1;
      hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID = BXVD_IMAGE_FirmwareID_eAuthenticated_AVD1;
      
      hXvd->stPlatformInfo.stReg = s_stPlatformReg_7440_AVD1;	   
   }

   /*
    * These are the 7440 specific stripe width and stripe multiple values.
    * Width is defined in bxvd_platform.h and the multiple is defined in
    * bxvd_platform_7440.h
    */

   hXvd->uiDecode_StripeWidth = BXVD_P_AVD_INIT_STRIPE_WIDTH;
   hXvd->uiDecode_IPShimPfriRegVal = BXVD_P_AVD_INIT_STRIPE_MULTIPLE;
   
   BDBG_LEAVE(BXVD_P_InitRegPtrs_7440A0);
}

#endif /* BXVD_P_USE_INIT_REG_PTRS_7440A0 */

