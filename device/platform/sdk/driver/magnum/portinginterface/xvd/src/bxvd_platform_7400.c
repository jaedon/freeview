/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7400.c $
 * $brcm_Revision: Hydra_Software_Devel/59 $
 * $brcm_Date: 7/20/11 3:05p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7400.c $
 * 
 * Hydra_Software_Devel/59   7/20/11 3:05p davidp
 * SW7420-2001: Reorder header file includes.
 * 
 * Hydra_Software_Devel/58   7/13/10 4:26p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/57   4/1/10 3:00p davidp
 * SW7405-4160: Fix PM clock register field names.
 * 
 * Hydra_Software_Devel/56   10/16/09 5:29p davidp
 * SW7401-4332: Implement new AVD reset procedure.
 * 
 * Hydra_Software_Devel/55   6/10/09 5:18p davidp
 * PR55198: Add power management support to 7400 platform.
 * 
 * Hydra_Software_Devel/54   10/17/08 5:33p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/53   8/5/08 3:34p nilesh
 * PR45322: Move BCHP_DECODE_RVC references to platform specific register
 * structures
 * 
 * Hydra_Software_Devel/52   6/10/08 4:35p davidp
 * PR38955: Added bvnf_intr_context_base to platform register structure to
 * send to AVD FW.
 * 
 * Hydra_Software_Devel/51   5/22/08 4:54p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/50   5/14/08 12:31p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/49   1/29/08 7:11p davidp
 * PR39098: Remove unused UART setup register fields from core reve0
 * register structure.
 * 
 * Hydra_Software_Devel/48   1/25/08 7:01p davidp
 * PR38884: Add decode stripe width register reference pointers/mask/shift
 * items to register table to support 7405 B0 differences.
 * 
 * Hydra_Software_Devel/47   11/20/07 2:28p davidp
 * PR27168: Move setup of TEST_PORT_CTRL register to BSP module.
 * 
 * Hydra_Software_Devel/46   11/6/07 5:53p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/45   10/31/07 11:08a btosi
 * PR36545: added "CabacBinDepth" to s_stPlatformReg_7400B0_AVD0 and
 * s_stPlatformReg_7400B0_AVD1
 * 
 * Hydra_Software_Devel/44   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/43   8/21/07 4:21p davidp
 * PR34052: Access PCROffset_STC register thru platformInfo.stReg
 * structure.
 * 
 * Hydra_Software_Devel/42   8/21/07 3:04p davidp
 * PR34052: Remove nolonger used register addr pointers from BXVD_Handle,
 * use platform_info.stReg structure to reference registers
 * 
 * Hydra_Software_Devel/41   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/7   7/2/07 1:53p nilesh
 * PR29915: Consolidated platform memory allocation code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   6/19/07 12:12p nilesh
 * PR29915: Merge from mainline
 * 
 * Hydra_Software_Devel/39   6/12/07 11:36a nilesh
 * PR31900: Merged makefile cleanup to mainline to support bsettop
 * B_CONFIG_IMAGE build option
 * 
 * Hydra_Software_Devel/PR31900/1   6/7/07 10:57a nilesh
 * PR31900: Moved XVD inclusion of image code to makefile
 * 
 * Hydra_Software_Devel/38   6/4/07 9:29a pblanco
 * PR23100: Proxy checkin for Adrian Hulse's kernel mode change.
 * 
 * Hydra_Software_Devel/37   4/26/07 3:19p nilesh
 * PR29949: BAVD_MFD_Picture hHeap parameter is now explicitly set by DM
 * 
 * Hydra_Software_Devel/36   4/12/07 5:26p davidp
 * PR26188: Allocate Still Picture Buffer from private picture heap.
 * 
 * Hydra_Software_Devel/35   4/3/07 4:00p davidp
 * PR26188: Free picture buffers in BXVD_P_TearDownFWMemory_7400B0
 * 
 * Hydra_Software_Devel/34   3/19/07 4:11p davidp
 * PR26188: Use BMEM routines to allocate picture buffers from private
 * memory heap.
 * 
 * Hydra_Software_Devel/33   3/2/07 1:36p nilesh
 * PR26188: Merged 7400B0 bring-up branch to mainline.  Cleaned up heap
 * naming and usage.
 * 
 * Hydra_Software_Devel/PR26188/3   2/28/07 4:27p nilesh
 * PR26188: Fixed incorrect calculation of FW Context base phy addr on
 * 7400B0
 * 
 * Hydra_Software_Devel/PR26188/2   2/27/07 6:03p nilesh
 * PR26188: Remove hack to set picture buffer address on 7400B0
 * 
 * Hydra_Software_Devel/PR26188/1   2/27/07 4:05p nilesh
 * PR26188: 7400B0 Bring-Up
 * 
 * Hydra_Software_Devel/32   2/26/07 5:44p nilesh
 * PR26188: Fix 7400B0 memory setup issues
 * 
 * Hydra_Software_Devel/31   2/26/07 3:41p davidp
 * PR26188: Use correct heaps for picture buffers and FW code/context
 * memory
 * 
 * Hydra_Software_Devel/30   2/25/07 7:02a nilesh
 * PR26188: Fixed AVD0/AVD1 interrupt bits for 7400B0
 * 
 * Hydra_Software_Devel/29   2/6/07 5:04p gaurava
 * PR 26188: Fixed compile/link problem related to multiple defines.
 * 
 * Hydra_Software_Devel/28   2/6/07 9:15a pblanco
 * PR26188: 7400B0 code changes.
 * 
 * Hydra_Software_Devel/27   11/30/06 1:22p pblanco
 * PR26253: Cleaned up compiler warning about portability.
 * 
 * Hydra_Software_Devel/26   11/29/06 1:05p pblanco
 * PR26188: Conditionalize inclusion of the BXVD_Image code.
 * 
 * Hydra_Software_Devel/25   11/28/06 12:38p pblanco
 * PR26188: Final changes. Both A0 and B0 build properly now.
 * 
 * Hydra_Software_Devel/24   11/27/06 4:00p pblanco
 * PR26188: Sanity checkin. Fixed references to new RDB constants.
 * 
 * Hydra_Software_Devel/23   10/20/06 3:16p davidp
 * PR25038: Enabled HD still picture decode when BXVD_DecodeMode_eAll is
 * in decode mode list.
 * 
 * Hydra_Software_Devel/22   10/19/06 2:58p nilesh
 * PR22673: Removed redefinition of TEST_PORT_CONTROL_VALUE_ARC_[0/1]
 * 
 * Hydra_Software_Devel/21   9/22/06 4:45p davidp
 * PR22867: Add StillDecodemode arguement which is returned by
 * BXVD_P_StillPictureEnabled()
 * 
 * Hydra_Software_Devel/20   9/18/06 3:05p davidp
 * PR22817: Add HD still picture entry in 7400 decodeMode memory usage
 * table.
 * 
 * Hydra_Software_Devel/19   9/13/06 6:39p davidp
 * PR22817: Update CDB/ITB memory usage table, also FW decode mode memory
 * usage table.
 * 
 * Hydra_Software_Devel/18   9/7/06 4:30p davidp
 * PR22867: Create and use platform specfic FW memory configuration table.
 * 
 * Hydra_Software_Devel/17   8/23/06 4:18p davidp
 * PR22673: Cleanup BERR_TRACE usage
 * 
 * Hydra_Software_Devel/16   8/21/06 3:21p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/15   8/1/06 5:58p davidp
 * PR22673: Add verify watchdog fired platform routine.
 * 
 * Hydra_Software_Devel/14   8/1/06 12:26p davidp
 * PR22673: Init picture buffer heap pointer in xvd_to_vdc structure in
 * SetupFWMemory().
 * 
 * Hydra_Software_Devel/13   7/27/06 8:14p davidp
 * PR22763: Move chip specific send FW command code to platform files.
 * 
 * Hydra_Software_Devel/12   7/26/06 4:51p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 * 
 * Hydra_Software_Devel/11   7/24/06 10:59a davidp
 * PR22673: Added SetupFWBuffer platform routines.
 * 
 * Hydra_Software_Devel/10   7/21/06 12:02p pblanco
 * PR22673: Fixed bug referencing the proper area for userdata memory.
 * 
 * Hydra_Software_Devel/9   7/21/06 9:49a pblanco
 * PR22673: Added userdata offset to address conversion code.
 * 
 * Hydra_Software_Devel/8   7/20/06 6:20p davidp
 * PR22673: Add init register pointers and mask platform routines.
 * 
 * Hydra_Software_Devel/7   7/20/06 9:43a pblanco
 * PR22673: Fixed misnamed debug macros.
 * 
 * Hydra_Software_Devel/6   7/20/06 9:29a pblanco
 * PR22673: Added debugging macros and cleaned up the code (format only).
 * 
 * Hydra_Software_Devel/5   7/19/06 11:49a pblanco
 * PR22673: Fix code for 7400.
 * 
 * Hydra_Software_Devel/4   7/19/06 11:23a pblanco
 * PR22673: Added the remainder of bxvd_priv.c conditionalized code to
 * platform and core modules.
 * 
 * Hydra_Software_Devel/3   7/18/06 3:47p pblanco
 * PR22673: Fixed platform specific FW loader code.
 * 
 * Hydra_Software_Devel/2   7/18/06 3:30p pblanco
 * PR22673: Moved FW load to platform appropriate modules for 7400, 7401
 * and 7118.
 * 
 * Hydra_Software_Devel/1   7/18/06 12:10a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:53p nilesh
 * PR22673: Restructure on 97400
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

#include "bchp_decode_sint_0.h"
#include "bchp_decode_sint_1.h"
#include "bchp_decode_sint_oloop_0.h"
#include "bchp_decode_sint_oloop_1.h"
#include "bchp_decode_main_0.h"
#include "bchp_decode_main_1.h"

#include "bchp_decode_rvc_0.h"
#include "bchp_decode_rvc_1.h"
#include "bchp_clk.h"


BDBG_MODULE(BXVD_PLATFORM_7400);

#ifdef BXVD_P_USE_INIT_REG_PTRS_7400B0

static const BXVD_P_PlatformReg_RevE0 s_stPlatformReg_7400B0_AVD0 = 
{
   BCHP_SUN_TOP_CTRL_SW_RESET,                    /* SWReset */
   BCHP_SUN_TOP_CTRL_SW_RESET_avd0_sw_reset_MASK, /* SWResetAVDMask */	

   BCHP_AVD_INTR2_0_CPU_SET,                      /* CPUL2InterruptSet */
   BCHP_AVD_INTR2_0_CPU_CLEAR,                    /* CPUL2InterruptClear */
   BCHP_AVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */ 

   BCHP_BVNF_INTR2_3_AVD0_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
   BCHP_BVNF_INTR2_3_AVD0_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
   BCHP_BVNF_INTR2_3_AVD0_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */
   
   BCHP_XPT_PCROFFSET_STC0,                       /* XPT_PCROffset_STC */

   BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_AVD_INTR2_0_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_AVD_INTR2_0_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_AVD_INTR2_0_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_AVD_INTR2_0_VICH_REG_INTR,         /* VIChkrReg */
   BCHP_INT_ID_AVD_INTR2_0_VICH_SCB_WR_INTR,      /* VIChkrSCBWr */
   BCHP_INT_ID_AVD_INTR2_0_VICH_INST_RD_INTR,     /* VIChkrInstrRead */
	
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
   0xffffffff,                                    /* Unused - WD Timeout */
   BCHP_DECODE_CPUREGS2_0_REG_WATCHDOG_TMR,       /* OuterWatchdogTimer */
   BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_MBX,        /* OuterCPU2HostMailbox */
   BCHP_DECODE_CPUREGS2_0_REG_HST2CPU_MBX,        /* OuterHost2CPUMailbox */
   BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_STAT,       /* OuterCPU2HostStatus */

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */	

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
   BCHP_DECODE_MAIN_0_REG_MAINCTL,                       /* uiDecode_MainCtl */
   BCHP_DECODE_MAIN_0_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */

   BCHP_SUN_GISB_ARB_REQ_MASK,                    /* SunGisbArb_ReqMask */
   BCHP_SUN_GISB_ARB_REQ_MASK_avd0_MASK,          /* SunGisbArb_ReqMaskAVDMask */

   BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */

#if BXVD_P_POWER_MANAGEMENT
   (uint32_t)NULL,                                /* uiAVDCtrl */
   (uint32_t)NULL,                                /* uiAVDCtrl_PwrDnMask */
   BCHP_CLK_PM_CTRL_1,                            /* uiCoreClkCtrl; */
   BCHP_CLK_PM_CTRL_1_DIS_AVD0_200_CLK_MASK,      /* uiCoreClkCtrl_PwrDnMask */
   BCHP_CLK_PM_CTRL_1,                            /* uiSCBClkCtrl */
   BCHP_CLK_PM_CTRL_1_DIS_MEMC_16_0_200_CLK_MASK, /* uiSCBClkCtrl_PwrDnMask */
   BCHP_CLK_PM_CTRL,                              /* uiGISBClkCtrl */
   BCHP_CLK_PM_CTRL_DIS_AVD0_108M_CLK_MASK,       /* uiGISBClkCtrl_PwrDnMask */
#endif
};

static const BXVD_P_PlatformReg_RevE0 s_stPlatformReg_7400B0_AVD1 = 
{
   BCHP_SUN_TOP_CTRL_SW_RESET,                    /* SWReset */
   BCHP_SUN_TOP_CTRL_SW_RESET_avd1_sw_reset_MASK, /* SWResetAVDMask */	

   BCHP_AVD_INTR2_1_CPU_SET,                      /* CPUL2InterruptSet */
   BCHP_AVD_INTR2_1_CPU_CLEAR,                    /* CPUL2InterruptClear */
   BCHP_AVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

   BCHP_BVNF_INTR2_3_AVD1_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
   BCHP_BVNF_INTR2_3_AVD1_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
   BCHP_BVNF_INTR2_3_AVD1_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */

   BCHP_XPT_PCROFFSET_STC1,                       /* XPT_PCROffset_STC */

   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_AVD_INTR2_1_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_AVD_INTR2_1_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_AVD_INTR2_1_VICH_REG_INTR,         /* VIChkrReg */
   BCHP_INT_ID_AVD_INTR2_1_VICH_SCB_WR_INTR,      /* VIChkrSCBWr */
   BCHP_INT_ID_AVD_INTR2_1_VICH_INST_RD_INTR,     /* VIChkrInstrRead */
	
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
   0xffffffff,                                    /* Unused - WD Timeout */
   BCHP_DECODE_CPUREGS2_1_REG_WATCHDOG_TMR,       /* OuterWatchdogTimer */
   BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_MBX,        /* OuterCPU2HostMailbox */
   BCHP_DECODE_CPUREGS2_1_REG_HST2CPU_MBX,        /* OuterHost2CPUMailbox */
   BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_STAT,       /* OuterCPU2HostStatus */

   BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,  /* CabacBinDepth */	

   BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
   BCHP_DECODE_SINT_1_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
   BCHP_DECODE_SINT_OLOOP_1_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
   BCHP_DECODE_MAIN_1_REG_MAINCTL,                       /* uiDecode_MainCtl */
   BCHP_DECODE_MAIN_0_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */

   BCHP_SUN_GISB_ARB_REQ_MASK,                    /* SunGisbArb_ReqMask */
   BCHP_SUN_GISB_ARB_REQ_MASK_avd1_MASK,          /* SunGisbArb_ReqMaskAVDMask */

   BCHP_DECODE_RVC_1_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_1_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_1_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_1_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_1_REG_RVC_END,   /* RVC End */

#if BXVD_P_POWER_MANAGEMENT
   (uint32_t)NULL,                                /* uiAVDCtrl */
   (uint32_t)NULL,                                /* uiAVDCtrl_PwrDnMask */
   BCHP_CLK_PM_CTRL_1,                            /* uiCoreClkCtrl; */
   BCHP_CLK_PM_CTRL_1_DIS_AVD1_200_CLK_MASK,      /* uiCoreClkCtrl_PwrDnMask */
   BCHP_CLK_PM_CTRL_1,                            /* uiSCBClkCtrl */
   BCHP_CLK_PM_CTRL_1_DIS_MEMC_16_1_200_CLK_MASK, /* uiSCBClkCtrl_PwrDnMask */
   BCHP_CLK_PM_CTRL,                              /* uiGISBClkCtrl */
   BCHP_CLK_PM_CTRL_DIS_AVD1_108M_CLK_MASK,       /* uiGISBClkCtrl_PwrDnMask */
#endif
};

void BXVD_P_InitRegPtrs_7400B0( BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_InitRegPtrs_7400B0);

   if (hXvd->uDecoderInstance == 0)
   {
      hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID = BXVD_IMAGE_FirmwareID_eOuterELF_AVD0;
      hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = BXVD_IMAGE_FirmwareID_eInnerELF_AVD0;
      hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID = BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0;

      hXvd->stPlatformInfo.stReg = s_stPlatformReg_7400B0_AVD0;
   }
   else
   {
      /* Platform Info */
      hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID = BXVD_IMAGE_FirmwareID_eOuterELF_AVD1;
      hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = BXVD_IMAGE_FirmwareID_eInnerELF_AVD1;
      hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID = BXVD_IMAGE_FirmwareID_eAuthenticated_AVD1;

      hXvd->stPlatformInfo.stReg = s_stPlatformReg_7400B0_AVD1;	  
   }

   BDBG_LEAVE(BXVD_P_InitRegPtrs_7400B0);
}
#endif /* BXVD_P_USE_INIT_REG_PTRS_7400B0 */
