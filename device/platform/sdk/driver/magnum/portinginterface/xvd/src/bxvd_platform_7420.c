/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_7420.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 7/26/11 4:27p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_platform_7420.c $
 * 
 * Hydra_Software_Devel/13   7/26/11 4:27p davidp
 * SW7231-127: Move PM2 7420 flavor to core reve0 support files, this
 * method is also used on other platforms.
 * 
 * Hydra_Software_Devel/12   7/21/11 4:54p davidp
 * SW7231-127: Add platform specific BXVD_P_SetPowerState_7420 routine.
 * 
 * Hydra_Software_Devel/11   7/20/11 3:20p davidp
 * SW7420-2001: Reorder header-file includes.
 * 
 * Hydra_Software_Devel/10   7/13/10 4:29p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/9   4/1/10 3:00p davidp
 * SW7405-4160: Fix PM clock register field names.
 * 
 * Hydra_Software_Devel/8   12/1/09 9:48a btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_* at compile time.
 * 
 * Hydra_Software_Devel/7   9/18/09 5:22p davidp
 * SW7420-354: Use new AVD reset procedure.
 * 
 * Hydra_Software_Devel/6   9/16/09 5:32p davidp
 * SW3548-2458: Add AVD core hw block reset registers to register table.
 * 
 * Hydra_Software_Devel/5   7/29/09 5:12p davidp
 * PR57178: Change name of AVD 250mz clock for rev B0.
 * 
 * Hydra_Software_Devel/4   6/1/09 6:38p davidp
 * PR55198: Add AVD power management register pointers and masks to
 * register table.
 * 
 * Hydra_Software_Devel/3   1/21/09 2:02p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 * 
 * Hydra_Software_Devel/2   10/17/08 5:34p davidp
 * PR45799: Add Host L2 interrupt set register address to AVD FW init
 * command.
 * 
 * Hydra_Software_Devel/1   10/15/08 5:17p davidp
 * PR47748: Add support for 7420 platform
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
#include "bchp_clk.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif


BDBG_MODULE(BXVD_PLATFORM_7420);

#ifdef BXVD_P_USE_INIT_REG_PTRS_7420
static const BXVD_P_PlatformReg_RevI0 s_stPlatformReg_7420_AVD0 = 
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

   (uint32_t)NULL,                                   /* uiAVDCtrl */
   (uint32_t)NULL,                                   /* uiAVDCtrl_PwrDnMask */
   BCHP_CLK_AVD0_CLK_PM_CTRL,                        /* uiCoreClkCtrl; */
#if (BCHP_VER == BCHP_VER_A0) || (BCHP_VER == BCHP_VER_A1)
   BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_AVD0_PROG_CLK_MASK, /* uiCoreClkCtrl_PwrDnMask */
#else
   BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_250M_CLK_MASK, /* uiCoreClkCtrl_PwrDnMask */
#endif
   BCHP_CLK_AVD0_CLK_PM_CTRL,                        /* uiSCBClkCtrl */
   BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_216M_CLK_MASK,      /* uiSCBClkCtrl_PwrDnMask */
   BCHP_CLK_AVD0_CLK_PM_CTRL,                        /* uiGISBClkCtrl */
   BCHP_CLK_AVD0_CLK_PM_CTRL_DIS_108M_CLK_MASK,      /* uiGISBClkCtrl_PwrDnMask */
};

static const BXVD_P_PlatformReg_RevI0 s_stPlatformReg_7420_AVD1 = 
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

   BCHP_DECODE_CPUREGS_1_REG_INST_BASE,         /* InnerInstructionBase */
   BCHP_DECODE_CPUREGS_1_REG_END_OF_CODE,       /* InnerEndOfCode */
   BCHP_DECODE_CPUREGS_1_REG_GLOBAL_IO_BASE,    /* InnerGlobalIOBase */
   BCHP_DECODE_IND_SDRAM_REGS_1_REG_CPU_DBG,    /* InnerCPUDebug */
   BCHP_DECODE_CPUAUX_1_CPUAUX_REG,             /* InnerCPUAux */

   BCHP_DECODE_CPUREGS2_1_REG_INST_BASE,        /* OuterEndOfCode */
   BCHP_DECODE_CPUREGS2_1_REG_END_OF_CODE,      /* OuterInstructionBase */
   BCHP_DECODE_CPUREGS2_1_REG_GLOBAL_IO_BASE,   /* OuterGlobalIOBase */
   BCHP_DECODE_IND_SDRAM_REGS2_1_REG_CPU_DBG,   /* OuterCPUDebug */
   BCHP_DECODE_CPUAUX2_1_CPUAUX_REG,            /* OuterCPUAux */
   BCHP_DECODE_CPUREGS2_1_REG_CPU_INTGEN_MASK,  /* OuterInterruptMask */
   BCHP_DECODE_CPUREGS2_1_REG_CPU_INTGEN_CLR,   /* OuterInterruptClear */
   BCHP_DECODE_CPUREGS2_1_REG_WATCHDOG_TMR,     /* OuterWatchdogTimer */
   BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_MBX,      /* OuterCPU2HostMailbox */
   BCHP_DECODE_CPUREGS2_1_REG_HST2CPU_MBX,      /* OuterHost2CPUMailbox */
   BCHP_DECODE_CPUREGS2_1_REG_CPU2HST_STAT,     /* OuterCPU2HostStatus */

   BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */

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

   BCHP_DECODE_SD_1_REG_SD_STRIPE_WIDTH,                       /* StripeWidth */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Stripe_Width_MASK,     /* StripeWidthMask */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Stripe_Width_SHIFT,    /* StripeWidthShift */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_pfri_bank_height_MASK, /* PFRIBankHeightMask */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_pfri_bank_height_SHIFT, /* PFRIBankHeightShift */

   BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH,                       /* PFRIData */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_MASK,  /* PFRIDataBusWidthMask */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_SHIFT, /* PFRIDataBusWidthShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_MASK,      /* PFRIDataSourceMask */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_SHIFT,     /* PFRIDataSourceShift */

   BCHP_DECODE_RVC_1_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_1_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_1_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_1_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_1_REG_RVC_END,   /* RVC End */

   (uint32_t)NULL,                                   /* uiAVDCtrl */
   (uint32_t)NULL,                                   /* uiAVDCtrl_PwrDnMask */
   BCHP_CLK_AVD1_CLK_PM_CTRL,                        /* uiCoreClkCtrl; */
#if (BCHP_VER == BCHP_VER_A0) || (BCHP_VER == BCHP_VER_A1)
   BCHP_CLK_AVD1_CLK_PM_CTRL_DIS_AVD1_PROG_CLK_MASK, /* uiCoreClkCtrl_PwrDnMask */
#else
   BCHP_CLK_AVD1_CLK_PM_CTRL_DIS_250M_CLK_MASK, /* uiCoreClkCtrl_PwrDnMask */
#endif
   BCHP_CLK_AVD1_CLK_PM_CTRL,                        /* uiSCBClkCtrl */
   BCHP_CLK_AVD1_CLK_PM_CTRL_DIS_216M_CLK_MASK,      /* uiSCBClkCtrl_PwrDnMask */
   BCHP_CLK_AVD1_CLK_PM_CTRL,                        /* uiGISBClkCtrl */
   BCHP_CLK_AVD1_CLK_PM_CTRL_DIS_108M_CLK_MASK,      /* uiGISBClkCtrl_PwrDnMask */
};
#endif

#ifdef BXVD_P_USE_INIT_REG_PTRS_7420
void BXVD_P_InitRegPtrs_7420(BXVD_Handle hXvd)
{
   uint32_t uiDecode_DataWidth;
   uint32_t uiDecode_DataSource;

   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_InitRegPtrs_7420);

   /* Platform Info */
   if (hXvd->uDecoderInstance == 0)
   {
      hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID = BXVD_IMAGE_FirmwareID_eOuterELF_AVD0;
      hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = BXVD_IMAGE_FirmwareID_eInnerELF_AVD0;
      hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID = BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0;

      hXvd->stPlatformInfo.stReg = s_stPlatformReg_7420_AVD0;
   }
   else
   {
      hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID = BXVD_IMAGE_FirmwareID_eOuterELF_AVD1;
      hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = BXVD_IMAGE_FirmwareID_eInnerELF_AVD1;
      hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID = BXVD_IMAGE_FirmwareID_eAuthenticated_AVD1;

      hXvd->stPlatformInfo.stReg = s_stPlatformReg_7420_AVD1;	  
   }

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

   BDBG_LEAVE(BXVD_P_InitRegPtrs_7420);
}
#endif /* BXVD_P_USE_INIT_REG_PTRS_7420 */

#ifdef BXVD_P_USE_INIT_REG_PTRS_7420
BERR_Code BXVD_P_ChipReset_7420(BXVD_Handle hXvd)
{
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_ChipReset_7420);

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

   BDBG_LEAVE(BXVD_P_ChipReset_7420);

   return BERR_TRACE(BERR_SUCCESS);
}
#endif

