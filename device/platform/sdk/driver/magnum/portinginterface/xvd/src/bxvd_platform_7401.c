/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7401.c $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 7/20/11 3:05p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7401.c $
 * 
 * Hydra_Software_Devel/27   7/20/11 3:05p davidp
 * SW7420-2001: Reorder header file includes.
 * 
 * Hydra_Software_Devel/26   7/13/10 4:26p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/25   10/16/09 4:07p davidp
 * SW7401-4332: Implement new AVD reset procedure.
 * 
 * Hydra_Software_Devel/24   10/17/08 5:33p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/23   8/5/08 3:34p nilesh
 * PR45322: Move BCHP_DECODE_RVC references to platform specific register
 * structures
 * 
 * Hydra_Software_Devel/22   6/10/08 4:35p davidp
 * PR38955: Added bvnf_intr_context_base to platform register structure to
 * send to AVD FW.
 * 
 * Hydra_Software_Devel/21   5/22/08 4:52p davidp
 * PR41411: Add verification that the AVD can access GISB.
 * 
 * Hydra_Software_Devel/20   5/14/08 12:31p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/19   1/29/08 6:00p davidp
 * PR39098: Remove unused register items from revE0 register structure.
 * 
 * Hydra_Software_Devel/18   1/25/08 7:01p davidp
 * PR38884: Add decode stripe width register reference pointers/mask/shift
 * items to register table to support 7405 B0 differences.
 * 
 * Hydra_Software_Devel/17   11/6/07 5:53p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/16   10/31/07 11:08a btosi
 * PR36545: added "CabacBinDepth" to s_stPlatformReg_7401B0
 * 
 * Hydra_Software_Devel/15   8/21/07 5:41p davidp
 * PR34052: Access registers via platformInfo.stReg structure.
 * 
 * Hydra_Software_Devel/14   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   7/16/07 2:00p nilesh
 * PR29915: Added PicDataRdy Interrupt 1 definition
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   6/19/07 12:12p nilesh
 * PR29915: Merge from mainline
 * 
 * Hydra_Software_Devel/13   6/12/07 11:36a nilesh
 * PR31900: Merged makefile cleanup to mainline to support bsettop
 * B_CONFIG_IMAGE build option
 * 
 * Hydra_Software_Devel/PR31900/1   6/7/07 10:57a nilesh
 * PR31900: Moved XVD inclusion of image code to makefile
 * 
 * Hydra_Software_Devel/12   6/1/07 3:08p nilesh
 * PR23100: Fixed compilation on non-7401 platforms
 * 
 * Hydra_Software_Devel/11   6/1/07 12:04p pblanco
 * PR23100: Proxy checkin for Adrian Hulse's kernel mode change.
 * 
 * Hydra_Software_Devel/10   2/2/07 12:56p gaurava
 * PR 27283: New wat   interrupts for 7403/7405 and made them consistent
 * with 7401.
 * 
 * Hydra_Software_Devel/9   1/23/07 1:41p pblanco
 * PR27283: New UART initialization support.
 * 
 * Hydra_Software_Devel/8   11/17/06 4:51p nilesh
 * PR25868: Merged authenticated image support to mainline
 * 
 * Hydra_Software_Devel/PR25868/1   11/14/06 3:55p nilesh
 * PR25868: Added pre-relocated image support
 * 
 * Hydra_Software_Devel/7   11/13/06 2:03p nilesh
 * PR25365: Merged BIMG support to mainline
 * 
 * Hydra_Software_Devel/PR25365/2   11/10/06 2:16p nilesh
 * PR25365: Added explicit size to firmware image array so that
 * unsupported firmware entries are set to NULL implicitly
 * 
 * Hydra_Software_Devel/PR25365/1   11/10/06 11:05a nilesh
 * PR25365: Added BIMG support
 * 
 * Hydra_Software_Devel/6   9/5/06 3:04p davidp
 * PR22867: Merge secure heap (Cabac) support
 * 
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:11p davidp
 * PR22867: Separate FW memory so Cabac bin memory can me moved to secure
 * heap.
 * 
 * Hydra_Software_Devel/5   8/21/06 3:21p nilesh
 * PR22673: Fix for possible macro namespace conflicts
 * 
 * Hydra_Software_Devel/4   8/21/06 1:53p nilesh
 * PR22673: Restructure FW Load mechanism to use standard core firmware
 * struct definition
 * 
 * Hydra_Software_Devel/3   8/17/06 12:02p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/PR22673/3   8/16/06 3:23p nilesh
 * PR22673: Fix cross-platform build error
 * 
 * Hydra_Software_Devel/PR22673/2   8/16/06 3:10p nilesh
 * PR22673: Fixed BDBG typo
 * 
 * Hydra_Software_Devel/PR22673/1   8/16/06 2:01p nilesh
 * PR22673: Use register pointers instead of absolute addresses for all
 * register accesses from core code
 * 
 * Hydra_Software_Devel/2   7/20/06 9:29a pblanco
 * PR22673: Added debugging macros and cleaned up the code (format only).
 * 
 * Hydra_Software_Devel/1   7/18/06 12:11a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 * 
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:20p nilesh
 * PR22673: Code Restructure
 *
 ***************************************************************************/

#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_intr.h"
#include "bxvd_reg.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"
/* to pickup BCHP_REG_CABAC2BINS2_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE */
#include "bchp_reg_cabac2bins2.h"

#include "bchp_decode_ind_sdram_regs.h"
#include "bchp_decode_sint.h"
#include "bchp_decode_sint_oloop.h"
#include "bchp_decode_main.h"

#include "bchp_decode_rvc.h"

BDBG_MODULE(BXVD_PLATFORM_7401);

#ifdef BXVD_P_USE_INIT_REG_PTRS_7401B0

static const BXVD_P_PlatformReg_RevE0 s_stPlatformReg_7401B0 = 
{
	BCHP_SUN_TOP_CTRL_SW_RESET,                   /* SWReset */
	BCHP_SUN_TOP_CTRL_SW_RESET_avd_sw_reset_MASK, /* SWResetAVDMask */	

	BCHP_AVD0_INTR2_CPU_SET,                      /* CPUL2InterruptSet */
	BCHP_AVD0_INTR2_CPU_CLEAR,                    /* CPUL2InterruptClear */
	BCHP_AVD0_INTR2_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

        BCHP_BVNF_INTR2_3_AVD0_STATUS,                /* Bvnf_Intr2_3_AvdStatus */
        BCHP_BVNF_INTR2_3_AVD0_CLEAR,                 /* Bvnf_Intr2_3_AvdClear */
        BCHP_BVNF_INTR2_3_AVD0_MASK_CLEAR,            /* Bvnf_Intr2_3_AvdMaskClear */

        BCHP_XPT_PCROFFSET0_STC,                      /* XPT_PCROffset_STC */

	BCHP_INT_ID_AVD_SW_INTR10,  /* PicDataRdy */
        BCHP_INT_ID_AVD_SW_INTR13,  /* PicDataRdy1 */
	BCHP_INT_ID_AVD_SW_INTR11,  /* Mailbox */
	BCHP_INT_ID_AVD_SW_INTR14,  /* StereoSeqError */
	BCHP_INT_ID_AVD_SW_INTR12,  /* StillPictureRdy */
	BCHP_INT_ID_AVD_MBOX_INTR,  /* OuterWatchdog */
	BCHP_INT_ID_VICH_REG_INTR,     /* VIChkrReg */
	BCHP_INT_ID_VICH_SCB_WR_INTR,  /* VIChkrSCBWr */
	BCHP_INT_ID_VICH_INST_RD_INTR, /* VIChkrInstrRead */
	
	BCHP_DECODE_CPUREGS_REG_INST_BASE,           /* InnerInstructionBase */
	BCHP_DECODE_CPUREGS_REG_END_OF_CODE,         /* InnerEndOfCode */
	BCHP_DECODE_CPUREGS_REG_GLOBAL_IO_BASE,      /* InnerGlobalIOBase */
        BCHP_DECODE_IND_SDRAM_REGS_REG_CPU_DBG,      /* InnerCPUDebug */
        BCHP_DECODE_CPUAUX_CPUAUX_REG,               /* InnerCPUAux */


	BCHP_DECODE_CPUREGS2_REG_INST_BASE,          /* OuterEndOfCode */
	BCHP_DECODE_CPUREGS2_REG_END_OF_CODE,        /* OuterInstructionBase */
	BCHP_DECODE_CPUREGS2_REG_GLOBAL_IO_BASE,     /* OuterGlobalIOBase */
	BCHP_DECODE_IND_SDRAM_REGS2_REG_CPU_DBG,     /* OuterCPUDebug */
	BCHP_DECODE_CPUAUX2_CPUAUX_REG,              /* OuterCPUAux */
	BCHP_DECODE_CPUREGS2_REG_CPU_INTGEN_MASK,    /* OuterInterruptMask */
	BCHP_DECODE_CPUREGS2_REG_CPU_INTGEN_CLR,     /* OuterInterruptClear */
	BCHP_DECODE_CPUREGS2_REG_WATCHDOG_TMR_LIMIT, /* OuterWatchdogTimerLimit */
	BCHP_DECODE_CPUREGS2_REG_WATCHDOG_TMR,       /* OuterWatchdogTimer */
	BCHP_DECODE_CPUREGS2_REG_CPU2HST_MBX,        /* OuterCPU2HostMailbox */
	BCHP_DECODE_CPUREGS2_REG_HST2CPU_MBX,        /* OuterHost2CPUMailbox */
        BCHP_DECODE_CPUREGS2_REG_CPU2HST_STAT,       /* OuterCPU2HostStatus */

	BCHP_REG_CABAC2BINS2_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */

        BCHP_REG_CABAC2BINS2_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
        BCHP_REG_CABAC2BINS2_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
        BCHP_DECODE_SINT_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
        BCHP_DECODE_SINT_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
        BCHP_DECODE_SINT_OLOOP_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
        BCHP_DECODE_SINT_OLOOP_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
        BCHP_DECODE_MAIN_REG_MAINCTL,                       /* uiDecode_MainCtl */
        BCHP_DECODE_MAIN_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */

        BCHP_SUN_GISB_ARB_REQ_MASK,                    /* SunGisbArb_ReqMask */
        BCHP_SUN_GISB_ARB_REQ_MASK_req_mask_7_MASK,    /* SunGisbArb_ReqMaskAVDMask */

        BCHP_DECODE_RVC_REG_RVC_CTL,   /* RVC Control */
        BCHP_DECODE_RVC_REG_RVC_PUT,   /* RVC Put */
        BCHP_DECODE_RVC_REG_RVC_GET,   /* RVC Get */
        BCHP_DECODE_RVC_REG_RVC_BASE,  /* RVC Base */
        BCHP_DECODE_RVC_REG_RVC_END,   /* RVC End */
};

void BXVD_P_InitRegPtrs_7401B0(BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_InitRegPtrs_7401B0);

   /* Platform Info */
   hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID = BXVD_IMAGE_FirmwareID_eOuterELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = BXVD_IMAGE_FirmwareID_eInnerELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID = BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0;

   hXvd->stPlatformInfo.stReg = s_stPlatformReg_7401B0;

   BDBG_LEAVE(BXVD_P_InitRegPtrs_7401B0);
}

#endif /* BXVD_P_USE_INIT_REG_PTRS_7401B0 */

