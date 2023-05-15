/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7405.c $
 * $brcm_Revision: Hydra_Software_Devel/57 $
 * $brcm_Date: 7/25/11 3:08p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7405.c $
 * 
 * Hydra_Software_Devel/57   7/25/11 3:08p davidp
 * SW7231-127: Move old style PM support from 7405 specific to Rev E0
 * specific support file.
 * 
 * Hydra_Software_Devel/56   7/21/11 6:48a davidp
 * SW7231-127: Fix compiler warnings.
 * 
 * Hydra_Software_Devel/55   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/54   7/13/10 4:27p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/53   5/14/10 6:29p davidp
 * SW7125-426: Add dynamic power management support for 7125, disabled by
 * default.
 * 
 * Hydra_Software_Devel/52   4/1/10 3:00p davidp
 * SW7405-4160: Fix PM clock register field names.
 * 
 * Hydra_Software_Devel/51   12/1/09 9:48a btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile time.
 * 
 * Hydra_Software_Devel/50   11/16/09 6:33p davidp
 * SW7468-14: Add support for 7468 and 7408 platforms.
 * 
 * Hydra_Software_Devel/49   10/19/09 3:56p davidp
 * SW3548-2458: Use new reset procedure for AVD h0 rev chips.
 * 
 * Hydra_Software_Devel/48   9/16/09 5:05p davidp
 * SW3548-2458: Stop IL ARC and reset AVD core hw blocks in reset routine.
 * 
 * Hydra_Software_Devel/47   7/23/09 1:44p davidp
 * PR55546: Add 7125 support to xvd.
 * 
 * Hydra_Software_Devel/46   7/17/09 4:28p davidp
 * PR55227: Add support for 7340 and 7342 support.
 * 
 * Hydra_Software_Devel/45   6/2/09 3:16p davidp
 * PR55198: Always include power management header files.
 * 
 * Hydra_Software_Devel/44   6/1/09 6:38p davidp
 * PR55198: Add AVD power management register pointers and masks to
 * register table.
 * 
 * Hydra_Software_Devel/43   1/30/09 1:16p davidp
 * PR51623: Add support for 7336 chip, Rev I core clone.
 * 
 * Hydra_Software_Devel/42   1/21/09 2:02p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/41   12/23/08 1:19p davidp
 * PR50640: Add support for 7405 C0.
 * 
 * Hydra_Software_Devel/40   11/6/08 3:21p davidp
 * PR48622: Turn on VCX0 (250mhz) AVD clock, wait 1ms, then open clock
 * gate in 3548 hibernate routine.
 * 
 * Hydra_Software_Devel/39   11/4/08 6:07p davidp
 * PR48622: Leave GISB clocks on and wait 5ms after SCB clock is turned on
 * before AVD core clock is turned on.
 * 
 * Hydra_Software_Devel/38   10/17/08 5:32p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/37   10/3/08 5:29p davidp
 * PR46543: Add power management support for 3548/3556.
 * 
 * Hydra_Software_Devel/36   8/5/08 3:34p nilesh
 * PR45322: Move BCHP_DECODE_RVC references to platform specific register
 * structures
 * 
 * Hydra_Software_Devel/35   6/25/08 2:58p davidp
 * PR43119: Use atomic update routines for registers that are shared.
 * 
 * Hydra_Software_Devel/34   6/10/08 4:37p davidp
 * PR38955: Added uiBvnf_Intr2_3_AvdStatus to platform register structure
 * to send to AVD FW.
 * 
 * Hydra_Software_Devel/33   5/23/08 11:58a davidp
 * PR41411: Add GISB info to rev B0 and later too.
 * 
 * Hydra_Software_Devel/32   5/22/08 4:55p davidp
 * PR41411: Add SUB_GISB_ARB REQ_MASK register and AVD mask bit fields to
 * platform register structure.
 * 
 * Hydra_Software_Devel/31   5/14/08 12:36p davidp
 * PR21390: Add support for Aegis VICH interrupts.
 * 
 * Hydra_Software_Devel/30   2/20/08 6:48p davidp
 * PR39495: Add support for 3556, clone of 7405 using AVD core rev i0.
 * 
 * Hydra_Software_Devel/29   1/29/08 6:48p davidp
 * PR39098: Add support for AVD core rev i0.
 * 
 * Hydra_Software_Devel/28   1/25/08 7:02p davidp
 * PR38884: Add decode stripe width register reference pointers/mask/shift
 * items to register table to support 7405 B0 differences.
 * 
 * Hydra_Software_Devel/27   12/11/07 11:51a nilesh
 * PR27168: Fix compiler warning
 * 
 * Hydra_Software_Devel/26   11/20/07 2:28p davidp
 * PR27168: Move setup of TEST_PORT_CTRL register to BSP module.
 * 
 * Hydra_Software_Devel/25   11/6/07 5:55p davidp
 * PR27168: Remove use of BXVD_P_ChipEnable_RevE1 routine and associated
 * data structure, use RevE0
 * 
 * Hydra_Software_Devel/24   10/31/07 11:08a btosi
 * PR36545: added "CabacBinDepth" to s_stPlatformReg_7405A0
 * 
 * Hydra_Software_Devel/23   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 * 
 * Hydra_Software_Devel/22   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 * 
 * Hydra_Software_Devel/21   8/21/07 4:22p davidp
 * PR34052: Access PCROffset_STC register thru platformInfo.stReg
 * structure.
 * 
 * Hydra_Software_Devel/20   8/21/07 3:05p davidp
 * PR34052: Move register address pointers to platform_info.stReg
 * structure.
 * 
 * Hydra_Software_Devel/19   8/13/07 4:08p nilesh
 * PR29915: Multi-decode merge to mainline
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/9   8/9/07 3:50p nilesh
 * PR29915: Updated memory tables according to AVD Mosaic Doc v2.11 (Aug
 * 9, 2007)
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/7   7/17/07 11:36a nilesh
 * PR29915: Added PicDataRdy Interrupt 1 definition
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   7/13/07 5:15p nilesh
 * PR29915: Updated memory tables based on v2.9 of Mosaic API doc
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/5   7/10/07 6:46p nilesh
 * PR29915: Cleaned up 7405 platform code
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/4   7/10/07 6:18p nilesh
 * PR29915: Added 7405 support to mosaic branch
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   7/10/07 10:41a nilesh
 * PR29915: Merged from mainline 07/10/2007
 * 
 * Hydra_Software_Devel/14   7/6/07 7:41a pblanco
 * PR28215: Added chip specific reset routine with stripe/width register
 * reset.
 * 
 * Hydra_Software_Devel/13   6/28/07 3:57p pblanco
 * PR28215: Bringup changes - sanity checkin.
 * 
 * Hydra_Software_Devel/12   6/26/07 2:13p pblanco
 * PR28215: Added hardware write of stripe height and bank height to
 * BXVD_P_ChipEnable_7405.
 * 
 * Hydra_Software_Devel/11   6/22/07 11:26a pblanco
 * PR28215: Added platform specfic chip init code.
 * 
 * Hydra_Software_Devel/10   6/22/07 10:03a pblanco
 * PR28125: Remove hardwired debugging code.
 * 
 * Hydra_Software_Devel/9   6/21/07 2:39p pblanco
 * PR28215: Bringup-in-progress checkin.
 * 
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   6/19/07 12:13p nilesh
 * PR29915: Merge from mainline
 * 
 * Hydra_Software_Devel/8   6/12/07 11:37a nilesh
 * PR31900: Merged makefile cleanup to mainline to support bsettop
 * B_CONFIG_IMAGE build option
 * 
 * Hydra_Software_Devel/7   6/11/07 10:56a pblanco
 * PR28215: Use untranslated stripe width value in register init to
 * support VDC's requirements.
 * 
 * Hydra_Software_Devel/6   5/16/07 10:22a pblanco
 * PR28215: Fixed accidentally re-introduced header file includes.
 * 
 * Hydra_Software_Devel/5   5/16/07 10:10a pblanco
 * PR28215: Calculate the proper stripe width & height based on returned
 * register values.
 * 
 * Hydra_Software_Devel/5   5/16/07 10:04a pblanco
 * PR28215: Calculate the proper stripe width & height based on returned
 * register values.
 * 
 * Hydra_Software_Devel/3   5/15/07 1:19p pblanco
 * PR28215: Added support for variable stripe width and multiple.
 * 
 * Hydra_Software_Devel/2   4/19/07 10:37a pblanco
 * PR27645: Fixed register name build errors.
 * 
 * Hydra_Software_Devel/1   4/19/07 9:59a pblanco
 * PR27645: Adding to source control.
 * 
 ***************************************************************************/

#include "bxvd.h"
#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd_intr.h"
#include "bxvd_reg.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"
#include "bchp_common.h"


/* to pickup BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE */
#include "bchp_reg_cabac2bins2_0.h"
#include "bchp_decode_rvc_0.h"

/* Power management registers */
#include "bchp_vcxo_ctl_misc.h"

#if (BCHP_CHIP == 7405) || (BCHP_CHIP == 7335)
#include "bchp_clk.h"

#elif (BCHP_CHIP == 7325) || (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556) || (BCHP_CHIP == 7125)
#include "bchp_clkgen.h"
#endif

/* 
 * The following section is used to handle the slight register differences in the 7405 
 * family of chips. The 7405 family of chips can be grouped so to use the same register definitions. 
 * 
 * Here is the map of chips and revisions that are the same:
 *
 *       Register Rev Table                  Platforms
 *
 *          7405A0                       7405 A0 A1, 7335, 7325
 *
 *          7405B0                       7405 B0, 3548, 3556
 */ 

#if ((BCHP_CHIP == 7405) && ((BCHP_VER == BCHP_VER_A0) || (BCHP_VER == BCHP_VER_A1))) || \
    (BCHP_CHIP == 7335) || \
    (BCHP_CHIP == 7325)

/* Select 7405 A0 register definitions */
#define BXVD_P_USE_INIT_REG_PTRS_7405A0 1

#elif ((BCHP_CHIP == 7405) && (BCHP_VER >= BCHP_VER_B0)) || \
      (BCHP_CHIP == 3548) || \
      (BCHP_CHIP == 3556) || \
      (BCHP_CHIP == 7336) || \
      (BCHP_CHIP == 7340) || \
      (BCHP_CHIP == 7342) || \
      (BCHP_CHIP == 7125) || \
      (BCHP_CHIP == 7468) || \
      (BCHP_CHIP == 7408) 

/* Select 7405 B0 register definitions */
#define BXVD_P_USE_INIT_REG_PTRS_7405B0 1

#endif

BDBG_MODULE(BXVD_PLATFORM_7405);

#ifdef BXVD_P_USE_INIT_REG_PTRS_7405

#ifdef BXVD_P_USE_INIT_REG_PTRS_7405A0
static const BXVD_P_PlatformReg_RevH0 s_stPlatformReg_7405 = 
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

   BCHP_INT_ID_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_VICH_REG_INTR,         /* VIChkrReg */
   BCHP_INT_ID_VICH_SCB_WR_INTR,      /* VIChkrSCBWr */
   BCHP_INT_ID_VICH_INST_RD_INTR,     /* VIChkrInstrRead */

   BCHP_DECODE_CPUREGS_0_REG_INST_BASE,         /* InnerInstructionBase */
   BCHP_DECODE_CPUREGS_0_REG_END_OF_CODE,       /* InnerEndOfCode */
   BCHP_DECODE_CPUREGS_0_REG_GLOBAL_IO_BASE,    /* InnerGlobalIOBase */
   BCHP_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG,    /* InnerCPUDebug */
   BCHP_DECODE_CPUAUX_0_CPUAUX_REG,             /* InnerCPUAux */


   BCHP_DECODE_CPUREGS2_0_REG_INST_BASE,        /* OuterEndOfCode */
   BCHP_DECODE_CPUREGS2_0_REG_END_OF_CODE,      /* OuterInstructionBase */
   BCHP_DECODE_CPUREGS2_0_REG_GLOBAL_IO_BASE,   /* OuterGlobalIOBase */
   BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,   /* OuterCPUDebug */
   BCHP_DECODE_CPUAUX2_0_CPUAUX_REG,            /* OuterCPUAux */
   BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_MASK,  /* OuterInterruptMask */
   BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_CLR,   /* OuterInterruptClear */
   BCHP_DECODE_CPUREGS2_0_REG_WATCHDOG_TMR,     /* OuterWatchdogTimer */
   BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_MBX,      /* OuterCPU2HostMailbox */
   BCHP_DECODE_CPUREGS2_0_REG_HST2CPU_MBX,      /* OuterHost2CPUMailbox */
   BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_STAT,     /* OuterCPU2HostStatus */

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

   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH,                   /* StripeWidth */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Strip_Width_MASK,  /* StripeWidthMask */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Strip_Width_SHIFT, /* StripeWidthShift */
   BCHP_DECODE_IP_SHIM_0_PFRI_REG,                         /* PFRI */
   BCHP_DECODE_IP_SHIM_0_PFRI_REG_pfri_bank_height_MASK,   /* PFRIBankHeightMask */
   BCHP_DECODE_IP_SHIM_0_PFRI_REG_pfri_bank_height_SHIFT,  /* PFRIBankHeightShift */

   BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */

   BXVD_P_AVD0_CTRL,                      /* uiAVDCtrl */
   BXVD_P_AVD0_CTRL_PWRDN_MASK,           /* uiAVDCtrl_PwrDnMask */
   BXVD_P_AVD0_CORE_CLK_CTRL,             /* uiCoreClkCtrl; */
   BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK,  /* uiCoreClkCtrl_PwrDnMask */
   BXVD_P_AVD0_SCB_CLK_CTRL,              /* uiSCBClkCtrl */
   BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK,  /* uiSCBClkCtrl_PwrDnMask */
   BXVD_P_AVD0_GISB_CLK_CTRL,             /* uiGISBClkCtrl */
   BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK  /* uiGISBClkCtrl_PwrDnMask */
};
#endif

#ifdef BXVD_P_USE_INIT_REG_PTRS_7405B0
static const BXVD_P_PlatformReg_RevI0 s_stPlatformReg_7405 = 
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

   BCHP_INT_ID_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_VICH_REG_INTR,         /* VIChkrReg */
   BCHP_INT_ID_VICH_SCB_WR_INTR,      /* VIChkrSCBWr */
   BCHP_INT_ID_VICH_INST_RD_INTR,     /* VIChkrInstrRead */

   BCHP_DECODE_CPUREGS_0_REG_INST_BASE,         /* InnerInstructionBase */
   BCHP_DECODE_CPUREGS_0_REG_END_OF_CODE,       /* InnerEndOfCode */
   BCHP_DECODE_CPUREGS_0_REG_GLOBAL_IO_BASE,    /* InnerGlobalIOBase */
   BCHP_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG,    /* InnerCPUDebug */
   BCHP_DECODE_CPUAUX_0_CPUAUX_REG,             /* InnerCPUAux */

   BCHP_DECODE_CPUREGS2_0_REG_INST_BASE,        /* OuterEndOfCode */
   BCHP_DECODE_CPUREGS2_0_REG_END_OF_CODE,      /* OuterInstructionBase */
   BCHP_DECODE_CPUREGS2_0_REG_GLOBAL_IO_BASE,   /* OuterGlobalIOBase */
   BCHP_DECODE_IND_SDRAM_REGS2_0_REG_CPU_DBG,   /* OuterCPUDebug */
   BCHP_DECODE_CPUAUX2_0_CPUAUX_REG,            /* OuterCPUAux */
   BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_MASK,  /* OuterInterruptMask */
   BCHP_DECODE_CPUREGS2_0_REG_CPU_INTGEN_CLR,   /* OuterInterruptClear */
   BCHP_DECODE_CPUREGS2_0_REG_WATCHDOG_TMR,     /* OuterWatchdogTimer */
   BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_MBX,      /* OuterCPU2HostMailbox */
   BCHP_DECODE_CPUREGS2_0_REG_HST2CPU_MBX,      /* OuterHost2CPUMailbox */
   BCHP_DECODE_CPUREGS2_0_REG_CPU2HST_STAT,     /* OuterCPU2HostStatus */

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

   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH,                       /* StripeWidth */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Stripe_Width_MASK,     /* StripeWidthMask */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Stripe_Width_SHIFT,    /* StripeWidthShift */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_pfri_bank_height_MASK, /* PFRIBankHeightMask */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_pfri_bank_height_SHIFT, /* PFRIBankHeightShift */

   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH,                       /* PFRIData */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_MASK,  /* PFRIDataBusWidthMask */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_SHIFT, /* PFRIDataBusWidthShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_MASK,      /* PFRIDataSourceMask */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_SHIFT,     /* PFRIDataSourceShift */

   BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */

   BXVD_P_AVD0_CTRL,                      /* uiAVDCtrl */
   BXVD_P_AVD0_CTRL_PWRDN_MASK,           /* uiAVDCtrl_PwrDnMask */
   BXVD_P_AVD0_CORE_CLK_CTRL,             /* uiCoreClkCtrl; */
   BXVD_P_AVD0_CORE_CLK_CTRL_PWRDN_MASK,  /* uiCoreClkCtrl_PwrDnMask */
   BXVD_P_AVD0_SCB_CLK_CTRL,              /* uiSCBClkCtrl */
   BXVD_P_AVD0_SCB_CLK_CTRL_PWRDWN_MASK,  /* uiSCBClkCtrl_PwrDnMask */
   BXVD_P_AVD0_GISB_CLK_CTRL,             /* uiGISBClkCtrl */
   BXVD_P_AVD0_GISB_CLK_CTRL_PWRDWN_MASK  /* uiGISBClkCtrl_PwrDnMask */
};
#endif

#ifdef BXVD_P_USE_INIT_REG_PTRS_7405A0
void BXVD_P_InitRegPtrs_7405(BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_InitRegPtrs_7405);

   /* Platform Info */
   hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID =
      BXVD_IMAGE_FirmwareID_eOuterELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = 
      BXVD_IMAGE_FirmwareID_eInnerELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID =
      BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0;

   hXvd->stPlatformInfo.stReg = s_stPlatformReg_7405;

   /* 
    * Get stripe width and bank height values from their respective registers.
    */
   hXvd->uiDecode_SDStripeWidthRegVal = BXVD_Reg_Read32(hXvd, 
                                                        hXvd->stPlatformInfo.stReg.uiDecode_StripeWidth);
   
   hXvd->uiDecode_StripeWidth = ((hXvd->uiDecode_SDStripeWidthRegVal &
                                  hXvd->stPlatformInfo.stReg.uiDecode_StripeWidthMask) >>
                                 hXvd->stPlatformInfo.stReg.uiDecode_StripeWidthShift);
   
   hXvd->uiDecode_IPShimPfriRegVal = BXVD_Reg_Read32(hXvd, 
                                                     hXvd->stPlatformInfo.stReg.uiDecode_PFRI);
   
   hXvd->uiDecode_StripeMultiple = ((hXvd->uiDecode_IPShimPfriRegVal &
                                     hXvd->stPlatformInfo.stReg.uiDecode_PFRIBankHeightMask) >>
                                    hXvd->stPlatformInfo.stReg.uiDecode_PFRIBankHeightShift);
   
   BXVD_DBG_MSG(hXvd, ("stripe width = %d - stripe multiple = %d",
                            hXvd->uiDecode_StripeWidth, 
                            hXvd->uiDecode_StripeMultiple));

   BDBG_LEAVE(BXVD_P_InitRegPtrs_7405);
}
#endif

#ifdef BXVD_P_USE_INIT_REG_PTRS_7405B0
void BXVD_P_InitRegPtrs_7405(BXVD_Handle hXvd)
{
   uint32_t uiDecode_DataWidth;
   uint32_t uiDecode_DataSource;

   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_InitRegPtrs_7405);

   /* Platform Info */
   hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID =
      BXVD_IMAGE_FirmwareID_eOuterELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = 
      BXVD_IMAGE_FirmwareID_eInnerELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID =
      BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0;

   hXvd->stPlatformInfo.stReg = s_stPlatformReg_7405;

   /* 
    * Get stripe width and bank height values from their respective registers.
    */
   hXvd->uiDecode_SDStripeWidthRegVal = BXVD_Reg_Read32(hXvd, 
                                                        hXvd->stPlatformInfo.stReg.uiDecode_StripeWidth);

   hXvd->uiDecode_StripeWidth = (hXvd->uiDecode_SDStripeWidthRegVal &
                                  hXvd->stPlatformInfo.stReg.uiDecode_StripeWidthMask) >>
                                 hXvd->stPlatformInfo.stReg.uiDecode_StripeWidthShift;
   
   hXvd->uiDecode_StripeMultiple = ((hXvd->uiDecode_SDStripeWidthRegVal &
                                     hXvd->stPlatformInfo.stReg.uiDecode_PFRIBankHeightMask) >>
                                    hXvd->stPlatformInfo.stReg.uiDecode_PFRIBankHeightShift);
   
   hXvd->uiDecode_PFRIDataRegVal = BXVD_Reg_Read32(hXvd, 
                                               hXvd->stPlatformInfo.stReg.uiDecode_PFRIData);

   uiDecode_DataWidth = (hXvd->uiDecode_PFRIDataRegVal  & 
                         hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataBusWidthMask) >>
                        hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataBusWidthShift;

   uiDecode_DataSource = (hXvd->uiDecode_PFRIDataRegVal  & 
                         hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataSourceMask) >>
                        hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataSourceShift;

   BXVD_DBG_MSG(hXvd, ("PFRI Data width = %d - PFRI Data Source = %d",
                            uiDecode_DataWidth, uiDecode_DataSource));

   BXVD_DBG_MSG(hXvd, ("stripe width = %d - stripe multiple = %d",
                            hXvd->uiDecode_StripeWidth, 
                            hXvd->uiDecode_StripeMultiple));

   BDBG_LEAVE(BXVD_P_InitRegPtrs_7405);
}
#endif /* BXVD_P_USE_INIT_REG_PTRS_7405B0 */


#endif /* BXVD_P_USE_INIT_REG_PTRS_7405 */

#ifdef BXVD_P_USE_CORE_RESET_CHIP_7405

#ifdef BXVD_P_USE_INIT_REG_PTRS_7405A0
BERR_Code BXVD_P_ChipReset_7405(BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_ChipReset_7405);

   /* Reset Video Decoder */

   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiSun_SWReset,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask);

   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiSun_SWReset,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask,
                       0);

   /* Clear interrupt mask (Enable) register to prevent interrupts before ISR is setup */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptMask,
                    0);

   /* write the stripe with and bank height to hardware */

   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiDecode_StripeWidth,
                       0xffffffff,
                       hXvd->uiDecode_SDStripeWidthRegVal);


   BREG_AtomicUpdate32(hXvd->hReg,
                       hXvd->stPlatformInfo.stReg.uiDecode_PFRI,
                       0xffffffff,
                       hXvd->uiDecode_IPShimPfriRegVal);

   BDBG_LEAVE(BXVD_P_ChipReset_7405);

   return BERR_TRACE(BERR_SUCCESS);
}
#endif

#ifdef BXVD_P_USE_INIT_REG_PTRS_7405B0
BERR_Code BXVD_P_ChipReset_7405(BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_ChipReset_7405);


   /* Stop OL AVD ARC */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                    1);
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux,
                    0x02000000); 

   /* Stop IL AVD ARC */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerCPUDebug,
                    1);
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerCPUAux,
                    0x02000000); 

   /* Reset AVD HW blocks */
   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_CabacBinCtl,
                      hXvd->stPlatformInfo.stReg.uiDecode_CabacBinCtl_ResetMask,
                      "CABAC");

   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_SintStrmSts,
                      hXvd->stPlatformInfo.stReg.uiDecode_SintStrmSts_ResetMask, 
                      "Stream");

   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OLSintStrmSts,
                      hXvd->stPlatformInfo.stReg.uiDecode_OLSintStrmSts_ResetMask, 
                      "OLoopStream");

   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_MainCtl,
                      hXvd->stPlatformInfo.stReg.uiDecode_MainCtl_ResetMask,
                      "Backend" )

   /* Reset Video Decoder */

   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiSun_SWReset,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask);

   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiSun_SWReset,
                       hXvd->stPlatformInfo.stReg.uiSun_SWResetAVDMask,
                       0);

   /* Clear interrupt mask (Enable) register to prevent interrupts before ISR is setup */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptMask,
                    0);

   /* write the PFRI data source and data width values */
   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiDecode_PFRIData,
                       0xffffffff,
                       hXvd->uiDecode_PFRIDataRegVal);

   /* write the stripe with and bank height to hardware */

   BREG_AtomicUpdate32(hXvd->hReg, 
                       hXvd->stPlatformInfo.stReg.uiDecode_StripeWidth,
                       0xffffffff,
                       hXvd->uiDecode_SDStripeWidthRegVal);

   BDBG_LEAVE(BXVD_P_ChipReset_7405);

   return BERR_TRACE(BERR_SUCCESS);
}
#endif
#endif /* BXVD_P_USE_CORE_CHIP_RESET_7405 */



