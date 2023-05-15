/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_platform_revk0.c $
 * $brcm_Revision: Hydra_Software_Devel/39 $
 * $brcm_Date: 8/24/12 5:51p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd_platform_revk0.c $
 * 
 * Hydra_Software_Devel/39   8/24/12 5:51p davidp
 * SW7425-3618: Replace uncached memory accesses with cached memory
 * accesses.
 * 
 * Hydra_Software_Devel/38   4/17/12 2:21p davidp
 * SW7552-254: Add support for 8GBit memory configuration.
 * 
 * Hydra_Software_Devel/37   2/16/12 6:14p davidp
 * SW7425-2419: Enable core clock early on in chip reset routine.
 * 
 * Hydra_Software_Devel/36   2/3/12 3:17p davidp
 * SW7425-2024: Define BXVD_P OTP register contants to build on platforms
 * without the decoder OTP register.
 * 
 * Hydra_Software_Devel/35   2/2/12 3:03p davidp
 * SW7425-2024: Validate decode protocol specified in BXVD_StartDecode is
 * supported on platform.
 * 
 * Hydra_Software_Devel/34   1/6/12 5:25p davidp
 * SW7346-630: Add BXVD_P_VerifyWatchdogFired_RevK0 to read ARC600 PC
 * addresses properly.
 * 
 * Hydra_Software_Devel/33   11/9/11 6:36p davidp
 * SW7425-1683: Add support for 7425 single memory controller
 * configurations.
 * 
 * Hydra_Software_Devel/32   8/8/11 5:49p davidp
 * SW7552-75: Convert BCHP_GetFeature core clock value to Hz.
 * 
 * Hydra_Software_Devel/31   7/28/11 3:35p davidp
 * SW7552-75: Add BCHP_GetFeature call to determins AVD core clock freq on
 * platforms where freq varies.
 * 
 * Hydra_Software_Devel/30   7/25/11 4:11p davidp
 * SW7346-336: Remove deadcode in InitRegPtrs_RevK0, 64bit bus not
 * supported.
 * 
 * Hydra_Software_Devel/29   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/28   6/27/11 4:18p davidp
 * SW7425-542: Merge devel branch to mainline.
 * 
 * Hydra_Software_Devel/SW7425-542/5   6/22/11 2:05p davidp
 * SW7425-542: Add GISB workaround in chip reset routine.
 * 
 * Hydra_Software_Devel/SW7425-542/4   6/16/11 5:09p davidp
 * SW7425-615: Merge mainline GISB error workaround code to branch.
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/7/11 5:44p davidp
 * SW7425-542: Add macros and support routines to handle old and new HIM
 * FW API.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/1/11 6:12p davidp
 * SW7425-542: Add Host Interface Memory Read and Write macros.
 * 
 * Hydra_Software_Devel/SW7425-542/1   5/31/11 3:06p davidp
 * SW7425-542: Add support for Host Interface Memory (ARC Scratch memory)
 * FW API changes.
 * 
 * Hydra_Software_Devel/27   6/22/11 3:16p davidp
 * SW7425-615: Move GISB workaround to chip reset routine.
 * 
 * Hydra_Software_Devel/26   6/16/11 4:30p davidp
 * SW7425-615: Add and use BXVD_P_GISB_ERR_WORKAROUND symbolic constant.
 * 
 * Hydra_Software_Devel/25   2/17/11 7:15p davidp
 * SW7422-22: Fix boot callback info assignments.
 * 
 * Hydra_Software_Devel/24   2/15/11 5:32p davidp
 * SW7422-22: Add support for AVD Boot callback and AVD Reset Callback.
 * 
 * Hydra_Software_Devel/23   2/9/11 5:33p davidp
 * SW7422-173: Use ARC600 specific register to halt processor.
 * 
 * Hydra_Software_Devel/22   1/24/11 5:34p davidp
 * SWBLURAY-23545: Add support for 7640 platform.
 * 
 * Hydra_Software_Devel/21   1/5/11 4:42p davidp
 * SW7231-8: Use correct register define for BLD PFRI Data Width register
 * for SVD_ONLY platforms.
 * 
 * Hydra_Software_Devel/20   12/15/10 5:25p davidp
 * SW7422-149: Use constant definitions for FW IL and BL load offsets.
 * 
 * Hydra_Software_Devel/19   12/13/10 7:12p davidp
 * SW7344-5: Use MEMC_0 for decoder 0 on SVD only systems.
 * 
 * Hydra_Software_Devel/18   12/9/10 5:13p davidp
 * SW7422-22: Fix SVC support, allocate FW memory for AVD.
 * 
 * Hydra_Software_Devel/17   11/30/10 5:57p davidp
 * SW7422-112: Enable all AVD core clocks before resetting AVD cores.
 * 
 * Hydra_Software_Devel/16   11/22/10 11:02p davidp
 * SW7344-5: Fix BINT_IDs for SVD only platforms.
 * 
 * Hydra_Software_Devel/15   11/18/10 5:06p davidp
 * SW7422-22: Change atomicUpdate register writes to direct register
 * writes.
 * 
 * Hydra_Software_Devel/14   11/12/10 4:51p davidp
 * SW7422-22: Determine memory configuration in InitRegPtrs.
 * 
 * Hydra_Software_Devel/13   11/9/10 6:33p davidp
 * SW7422-22: Change name of DDR3 Capable flag.
 * 
 * Hydra_Software_Devel/12   11/8/10 4:26p davidp
 * SW7422-22: Fix Base Layer loading.
 * 
 * Hydra_Software_Devel/11   10/27/10 5:34p davidp
 * SW7422-22: Add VICH IL and BL interrupt support, add external boot
 * callback support.
 * 
 * Hydra_Software_Devel/10   10/25/10 3:56p davidp
 * SW7422-22: Set stripe reg settings based on bus width and memory part
 * size.
 * 
 * Hydra_Software_Devel/9   10/21/10 2:58p davidp
 * SW7231-8: Add support for 7231 and 7135 platforms.
 * 
 * Hydra_Software_Devel/8   10/20/10 3:05p davidp
 * SW7425-16: Set inner loop code size based on SVC present.
 * 
 * Hydra_Software_Devel/7   10/15/10 7:05p davidp
 * SW7425-16: Fix compiler warning.
 * 
 * Hydra_Software_Devel/6   10/15/10 6:40p davidp
 * SW7425-16: Add support for base layer ARC.
 * 
 * Hydra_Software_Devel/5   10/7/10 3:32p davidp
 * SW7422-22: Add new reset procedure with BLD support.
 * 
 * Hydra_Software_Devel/4   9/29/10 2:48p davidp
 * SW7422-22: Add BLD specific registers to register table.
 * 
 * Hydra_Software_Devel/3   9/23/10 3:39p davidp
 * SW7422-22: Add initial support for 7422.
 * 
 * Hydra_Software_Devel/2   9/22/10 5:48p davidp
 * SW7358-11: Fix AVD reset and add Rev K specific chip enable routine.
 * 
 * Hydra_Software_Devel/1   9/10/10 5:46p davidp
 * SW7358-11: Add support for AVD Rev K core.
 * 
 ***************************************************************************/

#include "bxvd_platform.h"
#include "bxvd_priv.h"
#include "bxvd.h"
#include "bxvd_intr.h"
#include "bxvd_reg.h"
#include "bxvd_image.h"
#include "bxvd_image_priv.h"
#include "bafl.h"
#include "bchp_common.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif


#if BXVD_P_SVD_GISB_ERR_WORKAROUND

/* SW7425-628: need for the work around for the bus error caused by register reads. */
#include "bchp_svd_rgr_0.h"

#endif


/* Define BXVD_POLL_FW_MBX to bring up decoder witout the use in interrupts */
/* #define BXVD_POLL_FW_MBX 1 */

/* ARC AUX register offsets */
#define BXVD_P_ARC_PC 0x18
#define BXVD_P_ARC_STATUS32 0x28

BDBG_MODULE(BXVD_PLATFORM_REVK0);

#ifdef BXVD_P_USE_INIT_REG_PTRS_REVK0

extern unsigned int BXVD_IMG_ELF_DecOuterCode_size; 
extern unsigned int BXVD_IMG_ELF_DecOuterData_size;
extern unsigned int BXVD_IMG_ELF_DecInnerCode_size; 
extern unsigned int BXVD_IMG_ELF_DecInnerData_size;

#if BXVD_P_SVD_AND_AVD_PRESENT

extern unsigned int BXVD_IMG_ELF_DecBaseCode_size; 
extern unsigned int BXVD_IMG_ELF_DecBaseData_size;

static const BXVD_P_PlatformReg_RevK0 s_stPlatformReg_SVD0RevK0 = 
{
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,                     /* SWInitSet */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_svd0_sw_init_MASK,   /* SWInitSetAVDMask */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,                   /* SWInitClear */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR_svd0_sw_init_MASK, /* SWInitClearAVDMask */ 

   BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG,             /* SoftShutdownCtrl */
   BCHP_DECODE_IP_SHIM_1_SOFTSHUTDOWN_CTRL_REG_Enable_MASK, /* SoftShutdownCtrlMask */

   BCHP_DECODE_IP_SHIM_0_REG_AVD_CLK_GATE,             /* IPShim_AvdClkGate */
   BCHP_DECODE_IP_SHIM_0_OTP_CTL_REG,                  /* IPShim_OtpCtl */
   BCHP_DECODE_IP_SHIM_1_OTP_CTL_REG_disable_RV9_MASK, /* IPShim_OtpCtl_DisableRV9 */
   BCHP_DECODE_IP_SHIM_0_CPU_ID,                  /* CpuId */
   BCHP_DECODE_IP_SHIM_1_CPU_ID_BLD_ID_MASK,      /* CpuId_BldIdMask */

   BCHP_SVD_INTR2_0_CPU_SET,                      /* CPUL2InterruptSet */
   BCHP_SVD_INTR2_0_CPU_CLEAR,                    /* CPUL2InterruptClear */
   BCHP_SVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

   BCHP_BVNF_INTR2_3_SVD0_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
   BCHP_BVNF_INTR2_3_SVD0_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
   BCHP_BVNF_INTR2_3_SVD0_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */

   BCHP_INT_ID_SVD_INTR2_0_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_SVD_INTR2_0_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_SVD_INTR2_0_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_SVD_INTR2_0_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_SVD_INTR2_0_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_SVD_INTR2_0_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_SVD_INTR2_0_VICH_REG_INTR,         /* VICReg */
   BCHP_INT_ID_SVD_INTR2_0_VICH_SCB_WR_INTR,      /* VICSCBWr */
   BCHP_INT_ID_SVD_INTR2_0_VICH_OL_INST_RD_INTR,  /* VICInstrRead */
   BCHP_INT_ID_SVD_INTR2_0_VICH_IL_INST_RD_INTR,  /* VICILInstrRead */
   BCHP_INT_ID_VICH_BL_INST_RD_INTR,              /* VICBLInstrRead */

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

   BCHP_BLD_DECODE_CPUREGS_0_REG_INST_BASE,      /* BaseInstructionBase */
   BCHP_BLD_DECODE_CPUREGS_0_REG_END_OF_CODE,    /* BaseEndOfCode */
   BCHP_BLD_DECODE_CPUREGS_0_REG_GLOBAL_IO_BASE, /* BaseGlobalIOBase */
   BCHP_BLD_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG, /* BaseCPUDebug */
   BCHP_BLD_DECODE_CPUAUX_0_CPUAUX_REG,          /* BaseCPUAux */

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */	

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
   BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
   BCHP_DECODE_SINT_1_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
   BCHP_DECODE_SINT_OLOOP_1_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
   BCHP_DECODE_MAIN_0_REG_MAINCTL,                       /* uiDecode_MainCtl */
   BCHP_DECODE_MAIN_1_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */
   BCHP_BLD_DECODE_IP_SHIM_0_SW_RESET_REG,              /* SWReset */
   BCHP_DECODE_IP_SHIM_1_SW_RESET_REG_ILSI_reset_MASK,  /* SWReset_ILSIResetMask */


   BCHP_BLD_DECODE_SINT_0_REG_SINT_STRM_STAT,           /* uiDecode_BLD_SintStrmSts */
   BCHP_DECODE_SINT_1_REG_SINT_STRM_STAT_Rst_MASK,      /* uiDecode_BLD_SintStrmSts_ResetMask */
   BCHP_ILS_REGS_0_ILS_SCALE_MODE,                      /* uiDecode_BLD_ILSScale */
   BCHP_ILS_REGS_0_ILS_SCALE_MODE_ILS_Rst_MASK,         /* uiDecode_BLD_ILSScale_ResetMask */
   BCHP_BLD_DECODE_MAIN_0_REG_MAINCTL,                  /* uiDecode_BLD_MainCtl */
   BCHP_DECODE_MAIN_1_REG_MAINCTL_Rst_MASK,             /* uiDecode_BLD_MainCtl_ResetMask */
   BCHP_BLD_DECODE_IP_SHIM_0_SW_RESET_REG,              /* uiDecode_BLD_SWReset */
   BCHP_DECODE_IP_SHIM_1_SW_RESET_REG_ILSI_reset_MASK,  /* uiDecode_BLD_SWReset_ILSIResetMask */

   BCHP_SUN_GISB_ARB_REQ_MASK,                     /* SunGisbArb_ReqMask */
   BCHP_SUN_GISB_ARB_REQ_MASK_svd_0_MASK,          /* SunGisbArb_ReqMaskAVDMask */

   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH,                       /* StripeWidth */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Stripe_Width_SHIFT,    /* StripeWidthShift */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_pfri_bank_height_SHIFT, /* PFRIBankHeightShift */

   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH,                       /* PFRIData */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_ddr3_mode_SHIFT,       /* PFRIDataDDR3ModeShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_SHIFT, /* PFRIDataBusWidthShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_SHIFT,     /* PFRIDataSourceShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_MASK,      /* PFRIDataSourceMask */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri2_source_SHIFT,    /* PFRIData2SourceShift */

   BCHP_BLD_DECODE_SD_0_REG_SD_STRIPE_WIDTH,                      /* BLStripeWidth */
   BCHP_BLD_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH,                   /* BLPFRIData */

   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0,             /* uiAVD_PcacheMode */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Ygran_MASK,  /* uiAVD_PcacheModeYGranMask */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Ygran_SHIFT, /* uiAVD_PcacheModeYGranShift */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Xgran_MASK,  /* uiAVD_PcacheModeXGranMask */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Xgran_SHIFT, /* uiAVD_PcacheModeXGranShift */

   BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */

   BCHP_DECODE_IP_SHIM_0_HST_SCRATCH_RAM_ADDR_REG, /* uiDecode_HST_SCR_RAM_ADDR */
   BCHP_DECODE_IP_SHIM_0_HST_SCRATCH_RAM_DATA_REG  /* uiDecode_HST_SCR_RAM_DATA */
};


static const BXVD_P_PlatformReg_RevK0 s_stPlatformReg_AVD1RevK0 = 
{
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,                     /* SWInitSet */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_avd0_sw_init_MASK,   /* SWInitSetAVDMask */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,                   /* SWInitClear */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR_avd0_sw_init_MASK, /* SWInitClearAVDMask */ 

   BCHP_DECODE_IP_SHIM_1_SOFTSHUTDOWN_CTRL_REG,             /* SoftShutdownCtrl */
   BCHP_DECODE_IP_SHIM_1_SOFTSHUTDOWN_CTRL_REG_Enable_MASK, /* SoftShutdownCtrlMask */

   BCHP_DECODE_IP_SHIM_1_REG_AVD_CLK_GATE,             /* IPShim_AvdClkGate */
   BCHP_DECODE_IP_SHIM_1_OTP_CTL_REG,                  /* IPShim_OtpCtl */
   BCHP_DECODE_IP_SHIM_1_OTP_CTL_REG_disable_RV9_MASK, /* IPShim_OtpCtl_DisableRV9 */
   BCHP_DECODE_IP_SHIM_1_CPU_ID,                  /* CpuId */
   BCHP_DECODE_IP_SHIM_1_CPU_ID_BLD_ID_MASK,      /* CpuId_BldIdMask */

   BCHP_AVD_INTR2_1_CPU_SET,                      /* CPUL2InterruptSet */
   BCHP_AVD_INTR2_1_CPU_CLEAR,                    /* CPUL2InterruptClear */
   BCHP_AVD_INTR2_1_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

   BCHP_BVNF_INTR2_3_AVD1_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
   BCHP_BVNF_INTR2_3_AVD1_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
   BCHP_BVNF_INTR2_3_AVD1_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */

   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_AVD_INTR2_1_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_AVD_INTR2_1_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_AVD_INTR2_1_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_AVD_INTR2_1_VICH_REG_INTR,         /* VICReg */
   BCHP_INT_ID_AVD_INTR2_1_VICH_SCB_WR_INTR,      /* VICSCBWr */
   BCHP_INT_ID_AVD_INTR2_1_VICH_OL_INST_RD_INTR,  /* VICInstrRead */
   BCHP_INT_ID_AVD_INTR2_1_VICH_IL_INST_RD_INTR,  /* VICILInstrRead */
   (uint32_t) NULL,                               /* VICBLInstrRead doesn't exist on AVD only core */

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

   (uint32_t) NULL,   /* BaseInstructionBase doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseEndOfCode doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseGlobalIOBase doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseCPUDebug doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseCPUAux doesn't exist on AVD only decoder */

   BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */	

   BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
   BCHP_REG_CABAC2BINS2_1_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
   BCHP_DECODE_SINT_1_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
   BCHP_DECODE_SINT_1_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
   BCHP_DECODE_SINT_OLOOP_1_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
   BCHP_DECODE_SINT_OLOOP_1_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
   BCHP_DECODE_MAIN_1_REG_MAINCTL,                       /* uiDecode_MainCtl */
   BCHP_DECODE_MAIN_1_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */
   BCHP_DECODE_IP_SHIM_1_SW_RESET_REG,                  /* SWReset */
   BCHP_DECODE_IP_SHIM_1_SW_RESET_REG_ILSI_reset_MASK,  /* SWReset_ILSIResetMask */


   (uint32_t) NULL,  /* uiDecode_BLD_SintStrmSts */
   (uint32_t) NULL,  /* uiDecode_BLDSintStrmSts_ResetMask */
   (uint32_t) NULL,  /* uiDecode_BLD_ILSScale */
   (uint32_t) NULL,  /* uiDecode_BLD_ILSScale_ResetMask */
   (uint32_t) NULL,  /* uiDecode_BLD_MainCtl */
   (uint32_t) NULL,  /* uiDecode_BLD_MainCtl_ResetMask */
   (uint32_t) NULL,  /* uiDecode_BLD_SWReset */
   (uint32_t) NULL,  /* uiDecode_BLD_SWReset_ILSIResetMask */

   BCHP_SUN_GISB_ARB_REQ_MASK,                     /* SunGisbArb_ReqMask */
   BCHP_SUN_GISB_ARB_REQ_MASK_avd_1_MASK,          /* SunGisbArb_ReqMaskAVDMask */

   BCHP_DECODE_SD_1_REG_SD_STRIPE_WIDTH,                          /* StripeWidth */
   BCHP_DECODE_SD_1_REG_SD_STRIPE_WIDTH_Stripe_Width_SHIFT,       /* StripeWidthShift */
   BCHP_DECODE_SD_1_REG_SD_STRIPE_WIDTH_pfri_bank_height_SHIFT,   /* PFRIBankHeightShift */

   BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH,                       /* PFRIData */
   BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH_ddr3_mode_SHIFT,       /* PFRIDataDDR3ModeShift */
   BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_SHIFT, /* PFRIDataBusWidthShift */
   BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH_pfri_source_SHIFT,     /* PFRIDataSourceShift */
   BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH_pfri_source_MASK,     /* PFRIDataSourceMASK */
   BCHP_DECODE_SD_1_REG_SD_PFRI_DATA_WIDTH_pfri2_source_SHIFT,    /* PFRIData2SourceShift */

   (uint32_t) NULL,            /* BLStripeWidth doesn't exist on AVD only core */
   (uint32_t) NULL,            /* BLPFRIData doesn't exist on AVD only core */

   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0,             /* uiAVD_PcacheMode */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Ygran_MASK,  /* uiAVD_PcacheModeYGranMask */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Ygran_SHIFT, /* uiAVD_PcacheModeYGranShift */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Xgran_MASK,  /* uiAVD_PcacheModeXGranMask */
   BCHP_AVD_CACHE_1_REG_PCACHE_MODE0_Xgran_SHIFT, /* uiAVD_PcacheModeXGranShift */

   BCHP_DECODE_RVC_1_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_1_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_1_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_1_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_1_REG_RVC_END,   /* RVC End */

   BCHP_DECODE_IP_SHIM_1_HST_SCRATCH_RAM_ADDR_REG, /* uiDecode_HST_SCR_RAM_ADDR */
   BCHP_DECODE_IP_SHIM_1_HST_SCRATCH_RAM_DATA_REG  /* uiDecode_HST_SCR_RAM_DATA */
};

#elif BXVD_P_SVD_ONLY_PRESENT 

extern unsigned int BXVD_IMG_ELF_DecBaseCode_size; 
extern unsigned int BXVD_IMG_ELF_DecBaseData_size;

static const BXVD_P_PlatformReg_RevK0 s_stPlatformReg_SVD0RevK0 = 
{
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,                     /* SWInitSet */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_svd0_sw_init_MASK,   /* SWInitSetAVDMask */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,                   /* SWInitClear */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR_svd0_sw_init_MASK, /* SWInitClearAVDMask */ 

   BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG,             /* SoftShutdownCtrl */
   BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG_Enable_MASK, /* SoftShutdownCtrlMask */

   BCHP_DECODE_IP_SHIM_0_REG_AVD_CLK_GATE,        /* IPShim_AvdClkGate */
   BXVD_P_OTP_CTL_REG,                            /* IPShim_OtpCtl */
   BXVD_P_OTP_CTL_REG_disable_RV9_MASK,           /* IPShim_OtpCtl_DisableRV9 */
   BCHP_DECODE_IP_SHIM_0_CPU_ID,                  /* CpuId */
   BCHP_DECODE_IP_SHIM_0_CPU_ID_BLD_ID_MASK,      /* CpuId_BldIdMask */

   BCHP_SVD_INTR2_0_CPU_SET,                      /* CPUL2InterruptSet */
   BCHP_SVD_INTR2_0_CPU_CLEAR,                    /* CPUL2InterruptClear */
   BCHP_SVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

   BCHP_BVNF_INTR2_3_SVD0_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
   BCHP_BVNF_INTR2_3_SVD0_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
   BCHP_BVNF_INTR2_3_SVD0_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */

   BCHP_INT_ID_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_VICH_REG_INTR,         /* VICReg */
   BCHP_INT_ID_VICH_SCB_WR_INTR,      /* VICSCBWr */
   BCHP_INT_ID_VICH_OL_INST_RD_INTR,  /* VICInstrRead */
   BCHP_INT_ID_VICH_IL_INST_RD_INTR,  /* VICILInstrRead */
   BCHP_INT_ID_VICH_BL_INST_RD_INTR,  /* VICBLInstrRead */

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

   BCHP_BLD_DECODE_CPUREGS_0_REG_INST_BASE,      /* BaseInstructionBase */
   BCHP_BLD_DECODE_CPUREGS_0_REG_END_OF_CODE,    /* BaseEndOfCode */
   BCHP_BLD_DECODE_CPUREGS_0_REG_GLOBAL_IO_BASE, /* BaseGlobalIOBase */
   BCHP_BLD_DECODE_IND_SDRAM_REGS_0_REG_CPU_DBG, /* BaseCPUDebug */
   BCHP_BLD_DECODE_CPUAUX_0_CPUAUX_REG,          /* BaseCPUAux */

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */	

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
   BCHP_DECODE_MAIN_0_REG_MAINCTL,                       /* uiDecode_MainCtl */
   BCHP_DECODE_MAIN_0_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */
   BCHP_BLD_DECODE_IP_SHIM_0_SW_RESET_REG,              /* SWReset */
   BCHP_DECODE_IP_SHIM_0_SW_RESET_REG_ILSI_reset_MASK,  /* SWReset_ILSIResetMask */


   BCHP_BLD_DECODE_SINT_0_REG_SINT_STRM_STAT,           /* uiDecode_BLD_SintStrmSts */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT_Rst_MASK,      /* uiDecode_BLD_SintStrmSts_ResetMask */
   BCHP_ILS_REGS_0_ILS_SCALE_MODE,                      /* uiDecode_BLD_ILSScale */
   BCHP_ILS_REGS_0_ILS_SCALE_MODE_ILS_Rst_MASK,         /* uiDecode_BLD_ILSScale_ResetMask */
   BCHP_BLD_DECODE_MAIN_0_REG_MAINCTL,                  /* uiDecode_BLD_MainCtl */
   BCHP_DECODE_MAIN_0_REG_MAINCTL_Rst_MASK,             /* uiDecode_BLD_MainCtl_ResetMask */
   BCHP_BLD_DECODE_IP_SHIM_0_SW_RESET_REG,              /* uiDecode_BLD_SWReset */
   BCHP_DECODE_IP_SHIM_0_SW_RESET_REG_ILSI_reset_MASK,  /* uiDecode_BLD_SWReset_ILSIResetMask */

   BCHP_SUN_GISB_ARB_REQ_MASK,                     /* SunGisbArb_ReqMask */
   BCHP_SUN_GISB_ARB_REQ_MASK_svd_0_MASK,          /* SunGisbArb_ReqMaskAVDMask */

   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH,                       /* StripeWidth */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Stripe_Width_SHIFT,    /* StripeWidthShift */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_pfri_bank_height_SHIFT, /* PFRIBankHeightShift */

   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH,                       /* PFRIData */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_ddr3_mode_SHIFT,       /* PFRIDataDDR3ModeShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_SHIFT, /* PFRIDataBusWidthShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_SHIFT,     /* PFRIDataSourceShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_MASK,      /* PFRIDataSourceMask */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri2_source_SHIFT,    /* PFRIData2SourceShift */

   BCHP_BLD_DECODE_SD_0_REG_SD_STRIPE_WIDTH,                      /* BLStripeWidth */
   BCHP_BLD_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH,                   /* BLPFRIData */

   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0,             /* uiAVD_PcacheMode */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Ygran_MASK,  /* uiAVD_PcacheModeYGranMask */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Ygran_SHIFT, /* uiAVD_PcacheModeYGranShift */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Xgran_MASK,  /* uiAVD_PcacheModeXGranMask */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Xgran_SHIFT, /* uiAVD_PcacheModeXGranShift */

   BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */

   BCHP_DECODE_IP_SHIM_0_HST_SCRATCH_RAM_ADDR_REG, /* uiDecode_HST_SCR_RAM_ADDR */
   BCHP_DECODE_IP_SHIM_0_HST_SCRATCH_RAM_DATA_REG  /* uiDecode_HST_SCR_RAM_DATA */
};

#else
static const BXVD_P_PlatformReg_RevK0 s_stPlatformReg_AVD0RevK0 = 
{
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,                     /* SWInitSet */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_SET_avd0_sw_init_MASK,   /* SWInitSetAVDMask */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,                   /* SWInitClear */
   BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR_avd0_sw_init_MASK, /* SWInitClearAVDMask */ 

   BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG,             /* SoftShutdownCtrl */
   BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG_Enable_MASK, /* SoftShutdownCtrlMask */

   BCHP_DECODE_IP_SHIM_0_REG_AVD_CLK_GATE,        /* IPShim_AvdClkGate */
   BXVD_P_OTP_CTL_REG,                            /* IPShim_OtpCtl */
   BXVD_P_OTP_CTL_REG_disable_RV9_MASK,           /* IPShim_OtpCtl_DisableRV9 */

   BCHP_DECODE_IP_SHIM_0_CPU_ID,                  /* CpuId */
   BCHP_DECODE_IP_SHIM_0_CPU_ID_BLD_ID_MASK,      /* CpuId_BldIdMask */

   BCHP_AVD_INTR2_0_CPU_SET,                      /* CPUL2InterruptSet */
   BCHP_AVD_INTR2_0_CPU_CLEAR,                    /* CPUL2InterruptClear */
   BCHP_AVD_INTR2_0_CPU_CLEAR_AVD_MBOX_INTR_MASK, /* CPUL2InterruptMailboxMask */

   BCHP_BVNF_INTR2_3_AVD0_STATUS,                 /* Bvnf_Intr2_3_AvdStatus */
   BCHP_BVNF_INTR2_3_AVD0_CLEAR,                  /* Bvnf_Intr2_3_AvdClear */
   BCHP_BVNF_INTR2_3_AVD0_MASK_CLEAR,             /* Bvnf_Intr2_3_AvdMaskClear */

   BCHP_INT_ID_AVD_SW_INTR0,          /* PicDataRdy */
   BCHP_INT_ID_AVD_SW_INTR3,          /* PicDataRdy1 */
   BCHP_INT_ID_AVD_MBOX_INTR,         /* Mailbox */
   BCHP_INT_ID_AVD_SW_INTR4,          /* StereoSeqError */
   BCHP_INT_ID_AVD_SW_INTR2,          /* StillPictureRdy */
   BCHP_INT_ID_AVD_WATCHDOG_INTR,     /* OuterWatchdog */
   BCHP_INT_ID_VICH_REG_INTR,         /* VICReg */
   BCHP_INT_ID_VICH_SCB_WR_INTR,      /* VICSCBWr */
   BCHP_INT_ID_VICH_OL_INST_RD_INTR,  /* VICInstrRead */
   BCHP_INT_ID_VICH_IL_INST_RD_INTR,  /* VICILInstrRead */
   (uint32_t) NULL,                   /* VICBLInstrRead doesn't exist on AVD only core */

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

   (uint32_t) NULL,   /* BaseInstructionBase doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseEndOfCode doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseGlobalIOBase doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseCPUDebug doesn't exist on AVD only decoder */
   (uint32_t) NULL,   /* BaseCPUAux doesn't exist on AVD only decoder */

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CHANNEL_WR_POSITION_i_ARRAY_BASE,    /* CabacBinDepth */	

   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL,            /* uiDecode_CabacBinCtl */
   BCHP_REG_CABAC2BINS2_0_REG_CABAC2BINS_CTL_Reset_MASK, /* uiDecode_CabacBinCtl_ResetMask */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT,                /* uiDecode_SintStrmSts */
   BCHP_DECODE_SINT_0_REG_SINT_STRM_STAT_Rst_MASK,       /* uiDecode_SintStrmSts_ResetMask */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT,          /* uiDecode_OLSintStrmSts */
   BCHP_DECODE_SINT_OLOOP_0_DEC_SINT_STRM_STAT_Rst_MASK, /* uiDecode_OLSintStrmSts_ResetMask */
   BCHP_DECODE_MAIN_0_REG_MAINCTL,                       /* uiDecode_MainCtl */
   BCHP_DECODE_MAIN_0_REG_MAINCTL_Rst_MASK,              /* uiDecode_MainCtl_ResetMask */
   BCHP_DECODE_IP_SHIM_0_SW_RESET_REG,                   /* uiDecode_SWReset */
   BCHP_DECODE_IP_SHIM_0_SW_RESET_REG_ILSI_reset_MASK,   /* uiDecode_SWReset_ILSIResetMask */

   /* The following Base Layer registers do not exist in AVD only core */
   (uint32_t) NULL,  /* uiDecode_BLD_SintStrmSts */
   (uint32_t) NULL,  /* uiDecode_BLD_SintStrmSts_ResetMask */
   (uint32_t) NULL,  /* uiDecode_BLD_ILSScale */
   (uint32_t) NULL,  /* uiDecode_BLD_ILSScale_ResetMask */
   (uint32_t) NULL,  /* uiDecode_BLD_MainCtl */
   (uint32_t) NULL,  /* uiDecode_BLD_MainCtl_ResetMask */
   (uint32_t) NULL,  /* uiDecode_BLD_SWReset */
   (uint32_t) NULL,  /* uiDecode_BLD_SWReset_ILSIResetMask */

#if (!BXVD_P_DVD_CHIP)
   BCHP_SUN_GISB_ARB_REQ_MASK,                     /* SunGisbArb_ReqMask */
   BCHP_SUN_GISB_ARB_REQ_MASK_avd_0_MASK,          /* SunGisbArb_ReqMaskAVDMask */

   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH,                          /* StripeWidth */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_Stripe_Width_SHIFT,       /* StripeWidthShift */
   BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH_pfri_bank_height_SHIFT,   /* PFRIBankHeightShift */

   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH,                       /* PFRIData */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_ddr3_mode_SHIFT,       /* PFRIDataDDR3ModeShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_data_width_SHIFT, /* PFRIDataBusWidthShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_SHIFT,     /* PFRIDataSourceShift */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri_source_MASK,      /* PFRIDataSourceMask */
   BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH_pfri2_source_SHIFT,    /* PFRIData2SourceShift */
#else
   BCHP_SUN_GISB_ARB_REQ_MASK,                     /* SunGisbArb_ReqMask */
   BCHP_SUN_GISB_ARB_REQ_MASK_avd0_MASK,           /* SunGisbArb_ReqMaskAVDMask */

   /* Stripe width and data register to not exist on DVD platforms. */
   (uint32_t) NULL, /* StripeWidth */
   (uint32_t) NULL, /* StripeWidthShift */
   (uint32_t) NULL,  /* PFRIBankHeightShift */

   (uint32_t) NULL,  /* PFRIData */
   (uint32_t) NULL,  /* PFRIDataDDR3ModeShift */
   (uint32_t) NULL,  /* PFRIDataBusWidthShift */
   (uint32_t) NULL,  /* PFRIDataSourceShift */
   (uint32_t) NULL,  /* PFRIDataSourceMask */
   (uint32_t) NULL,  /* PFRIData2SourceShift */
#endif

   (uint32_t) NULL,            /* BLStripeWidth doesn't exist on AVD only core */
   (uint32_t) NULL,            /* BLPFRIData doesn't exist on AVD only core */

   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0,             /* uiAVD_PcacheMode */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Ygran_MASK,  /* uiAVD_PcacheModeYGranMask */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Ygran_SHIFT, /* uiAVD_PcacheModeYGranShift */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Xgran_MASK,  /* uiAVD_PcacheModeXGranMask */
   BCHP_AVD_CACHE_0_REG_PCACHE_MODE0_Xgran_SHIFT, /* uiAVD_PcacheModeXGranShift */

   BCHP_DECODE_RVC_0_REG_RVC_CTL,   /* RVC Control */
   BCHP_DECODE_RVC_0_REG_RVC_PUT,   /* RVC Put */
   BCHP_DECODE_RVC_0_REG_RVC_GET,   /* RVC Get */
   BCHP_DECODE_RVC_0_REG_RVC_BASE,  /* RVC Base */
   BCHP_DECODE_RVC_0_REG_RVC_END,   /* RVC End */

   BCHP_DECODE_IP_SHIM_0_HST_SCRATCH_RAM_ADDR_REG, /* uiDecode_HST_SCR_RAM_ADDR */
   BCHP_DECODE_IP_SHIM_0_HST_SCRATCH_RAM_DATA_REG  /* uiDecode_HST_SCR_RAM_DATA */
};
#endif
#endif

#ifdef BXVD_P_USE_INIT_REG_PTRS_REVK0
void BXVD_P_InitRegPtrs_RevK0(BXVD_Handle hXvd)
{

#if (!BXVD_P_DVD_CHIP)
   uint32_t uiDataWidth;
   uint32_t uiDDRMode;
   uint32_t uiXGran=0;
   uint32_t uiYGran=0;
   bool     bDDR3Capable;
   uint32_t uiMemPartSize;
   uint32_t uiMemBusWidth;
   uint32_t pfriSource;
   uint32_t stripeWidth; 
   uint32_t bankHeight;
   uint32_t uiReg;
   uint32_t uiPCacheVal;

   BERR_Code rc = BERR_SUCCESS;
#endif

   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_InitRegPtrs_RevK0);

   /* Platform Info */
   hXvd->stPlatformInfo.stFirmware.outerELFFirmwareID =
      BXVD_IMAGE_FirmwareID_eOuterELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.innerELFFirmwareID = 
      BXVD_IMAGE_FirmwareID_eInnerELF_AVD0;
   hXvd->stPlatformInfo.stFirmware.authenticatedFirmwareID =
      BXVD_IMAGE_FirmwareID_eAuthenticated_AVD0;

#if BXVD_P_SVD_AND_AVD_PRESENT
   if (hXvd->uDecoderInstance == 0)
   {      
      hXvd->stPlatformInfo.stReg = s_stPlatformReg_SVD0RevK0;
   }
   else
   {      
      hXvd->stPlatformInfo.stReg = s_stPlatformReg_AVD1RevK0;
   }

#elif BXVD_P_SVD_ONLY_PRESENT
   hXvd->stPlatformInfo.stReg = s_stPlatformReg_SVD0RevK0;

#else /* Only AVD Present */
   hXvd->stPlatformInfo.stReg = s_stPlatformReg_AVD0RevK0;
#endif

#if BXVD_P_DYNAMIC_AVD_CORE_FREQ   

   /* Some platforms do not have a constant AVD Core clock speed */
   rc = BCHP_GetFeature(hXvd->hChip, BCHP_Feature_eAVDCoreFreq, &hXvd->uiAVDCoreFreq);

   if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("BCHP_GetFeature; feature not supported: BCHP_Feature_eAVDCoreFreq"));
         return;
      }

   BXVD_DBG_MSG(hXvd, ("AVD core clock freq: %d MHz", hXvd->uiAVDCoreFreq));

   hXvd->uiAVDCoreFreq *= 1000000;

#else

   hXvd->uiAVDCoreFreq = BXVD_P_AVD_CORE_UART_FREQ;

   BXVD_DBG_MSG(hXvd, ("AVD core clock freq: %d MHz", (hXvd->uiAVDCoreFreq/1000000)));

#endif

#if (!BXVD_P_DVD_CHIP)

/* 
 * By default on dual decoder (SVD and AVD) platforms, picture buffers use MEMC_1 memory.
 * If SVD is being forced to use MEMC_0 for picture buffers, then setup for MEMC_0 below. 
 */

#if (BXVD_P_SVD_AND_AVD_PRESENT && (!BXVD_P_SVD_USE_MEMC0))
   if (hXvd->uDecoderInstance == 0)
   {
      rc = BCHP_GetFeature(hXvd->hChip, BCHP_Feature_eMemCtrl1DDR3ModeCapable, &bDDR3Capable);
      if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("BCHP_GetFeature; feature not supported: eMemCtrl1DDR3ModeCapable"));
         return;
      }

      rc = BCHP_GetFeature(hXvd->hChip, BCHP_Feature_eMemCtrl1DDRDeviceTechCount, &uiMemPartSize);
      if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("BCHP_GetFeature; feature not supported: eMemCtrl1DDR3DeviceTechCount"));
         return;
      }

      rc = BCHP_GetFeature(hXvd->hChip, BCHP_Feature_eMemCtrl1DramWidthCount, &uiMemBusWidth);
      if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("BCHP_GetFeature; feature not supported: eMemCtrl1DramWidthCount"));
         return;
      }

      /* SVD0 PFRI attached to SCB1 */
      pfriSource=1;
   }
   else
#endif
   {
      /* AVD/SVD-Only Decoder or BXVD_P_SVD_USE_MEMC0 is enabled */

      BCHP_GetFeature(hXvd->hChip, BCHP_Feature_eMemCtrl0DDR3ModeCapable, &bDDR3Capable);
      if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("BCHP_GetFeature; feature not supported: eMemCtrl0DDR3ModeCapable"));
         return;
      }

      rc = BCHP_GetFeature(hXvd->hChip, BCHP_Feature_eMemCtrl0DDRDeviceTechCount, &uiMemPartSize);
      if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("BCHP_GetFeature; feature not supported: eMemCtrl0DDR3DeviceTechCount"));
         return;
      }

      rc = BCHP_GetFeature(hXvd->hChip, BCHP_Feature_eMemCtrl0DramWidthCount, &uiMemBusWidth);
      if (rc != BERR_SUCCESS)
      {
         BDBG_ERR(("BCHP_GetFeature; feature not supported: eMemCtrl0DramWidthCount"));
         return;
      }

      /* AVD/SVD-Only PFRI attached to SCB0 */
      pfriSource=0;
   }

   /* Set stripe width and bank height base on bus width and memory part size */
   switch(uiMemBusWidth)
   {
      /* Dram bus width 32 */
      case 32:

         switch(uiMemPartSize)
         {
            case 256: /* 256 Mbits Device Tech*/
               stripeWidth =0; /* 64 bytes */
               bankHeight = 1; /* 2 Mblks */
               break;

            case 512:  /* 512 Mbits Device Tech*/
            case 1024: /* 1024 Mbits Device Tech*/
            case 2048: /* 2048 Mbits Device Tech*/
            case 4096: /* 4096 Mbits Device Tech*/
            case 8192: /* 8192 Mbits Device Tech*/
               stripeWidth =1; /* 128 bytes */
               bankHeight = 1; /* 2 Mblks */
               break;

            default:
               stripeWidth = 0;
               bankHeight  = 1; /* 2 Mblks */
               BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
               break;
         }

         break;
         
      /* Dram bus width 32 */
      case 16:

         switch(uiMemPartSize)
         {
            case 256: /* 256 Mbits Device Tech*/
               stripeWidth =0; /* 64 bytes */
               bankHeight = 0; /* 1 Mblks */
               break;

            case 512:  /* 512 Mbits Device Tech*/
            case 1024: /* 1024 Mbits Device Tech*/
            case 2048: /* 2048 Mbits Device Tech*/
            case 4096: /* 4096 Mbits Device Tech*/
            case 8192: /* 8192 Mbits Device Tech*/
               stripeWidth =0; /* 64 bytes */
               bankHeight = 1; /* 2 Mblks */
               break;

            default:
               stripeWidth = 0; /* 64 bytes */
               bankHeight  = 0; /* 1 Mblks */
               BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
               break;
         }

         break;

      default:
         stripeWidth = 0;
         bankHeight  = 0;
         BDBG_ERR(("Unknown Value in DDR0 Device Config Register"));
         break;
   }

   hXvd->uiDecode_StripeWidth = stripeWidth;
   hXvd->uiDecode_StripeMultiple = bankHeight;
   
   uiReg = (stripeWidth << hXvd->stPlatformInfo.stReg.uiDecode_StripeWidthShift);
   uiReg |= (bankHeight << hXvd->stPlatformInfo.stReg.uiDecode_PFRIBankHeightShift);

   hXvd->uiDecode_SDStripeWidthRegVal = uiReg;

   if (bDDR3Capable == 1)
   {
      uiDDRMode = BXVD_P_PFRI_DATA_DDR3;
   }
   else
   {
      uiDDRMode = BXVD_P_PFRI_DATA_DDR2;
   }

   /* Only 16 or 32 Bit bus is supported */
   if (uiMemBusWidth == 16)
   {
      uiDataWidth = BXVD_P_PFRI_Data_Width_e16Bit;
   }
   else 
   {
      /* 32 bit bus */
      uiDataWidth = BXVD_P_PFRI_Data_Width_e32Bit;
   }

   uiReg = uiDataWidth << hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataBusWidthShift;
   uiReg |= ( pfriSource << hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataSourceShift);
   uiReg |= ( uiDDRMode << hXvd->stPlatformInfo.stReg.uiDecode_PFRIDataDDR3ModeShift);

   uiReg |= ( pfriSource << hXvd->stPlatformInfo.stReg.uiDecode_PFRIData2SourceShift);

   hXvd->uiDecode_PFRIDataRegVal = uiReg;

   if ( uiDDRMode == BXVD_P_PFRI_DATA_DDR2)
   {
      /* DDR2 memory configuration */
      switch (uiDataWidth)
      {
         case BXVD_P_PFRI_Data_Width_e16Bit:

            uiXGran = BXVD_P_PCache_XGran_e8Bytes;
            uiYGran = BXVD_P_PCache_YGran_e1Line;

            break;

         case BXVD_P_PFRI_Data_Width_e32Bit:

            uiXGran = BXVD_P_PCache_XGran_e8Bytes;
            uiYGran = BXVD_P_PCache_YGran_e2Lines;

            break;
      }
   }
   else
   {
      /* DDR3 memory configuration */
      switch ( uiDataWidth)
      {
         case BXVD_P_PFRI_Data_Width_e16Bit:

            uiXGran = BXVD_P_PCache_XGran_e16Bytes;
            uiYGran = BXVD_P_PCache_YGran_e1Line;

            break;

         case BXVD_P_PFRI_Data_Width_e32Bit:

            uiXGran = BXVD_P_PCache_XGran_e16Bytes;
            uiYGran = BXVD_P_PCache_YGran_e2Lines;

            break;
      }
   }

   uiPCacheVal = ( hXvd->uiAVD_PCacheRegVal & 
                   (~(hXvd->stPlatformInfo.stReg.uiAVD_PcacheModeYGranMask))&
                   (~(hXvd->stPlatformInfo.stReg.uiAVD_PcacheModeXGranMask)));

   hXvd->uiAVD_PCacheRegVal = uiPCacheVal | ((uiYGran << hXvd->stPlatformInfo.stReg.uiAVD_PcacheModeYGranShift) |
                                            (uiXGran << hXvd->stPlatformInfo.stReg.uiAVD_PcacheModeXGranShift));

#else

   hXvd->uiAVD_PCacheRegVal = BXVD_Reg_Read32(hXvd, 
                                              hXvd->stPlatformInfo.stReg.uiAVD_PcacheMode);
#endif

   BDBG_LEAVE(BXVD_P_InitRegPtrs_RevK0);
}
#endif /* BXVD_P_USE_INIT_REG_PTRS_REVK0 */

#ifdef BXVD_P_USE_CORE_RESET_CHIP_REVK0 
BERR_Code BXVD_P_ChipReset_RevK0(BXVD_Handle hXvd)
{
   uint32_t uiSoftDnVal;

   BERR_Code rc;
   
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_ChipReset_RevK0);

   /* Check to see if Decoder Core is shutdown */
   uiSoftDnVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrl);

   if (uiSoftDnVal == hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrlMask)
   {
      /* Turn Decoder on */
      BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrl, 0);
   }

   /* Enable the core clocks to be able to access core registers */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_IPShim_AvdClkGate,
                    0);

   /* Determine if this is a SVC or AVD decoder */
   if ( (BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_CPUId))
        & hXvd->stPlatformInfo.stReg.uiDecode_CPUId_BldIdMask)
   {
      hXvd->bSVCCapable = true;
   }

#if BXVD_P_RV9_CAPABLE 
  /* Determine if this system is RV9 capable */
   if (!(BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_IPShim_OtpCtl)
           & hXvd->stPlatformInfo.stReg.uiDecode_IPShim_OtpCtl_DisableRV9))
   {
      hXvd->bRV9Capable = true;
   }
   else
   {
      BXVD_DBG_WRN(hXvd, ("RV9 decode disabled by OTP"));
   }
#endif

   if (hXvd->stSettings.pAVDResetCallback)
   {
      /* Call external reset callback */
      rc = (*hXvd->stSettings.pAVDResetCallback)(hXvd->stSettings.pAVDResetCallbackData);

      if (rc != BERR_SUCCESS) 
      {
         BXVD_DBG_ERR(hXvd, ("Error #%d reseting the AVD externally", rc));
         return BERR_TRACE(rc);
      }
   }
   else
   {
      /* Stop OL AVD ARC */
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                       1);

      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux+BXVD_P_ARC_STATUS32,
                       1);

      /* Stop IL AVD ARC */
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_InnerCPUDebug,
                       1);
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_InnerCPUAux+BXVD_P_ARC_STATUS32,
                       1);

      if (hXvd->bSVCCapable)
      {
         /* Stop BL SVD ARC */
         BXVD_Reg_Write32(hXvd, 
                          hXvd->stPlatformInfo.stReg.uiDecode_BaseCPUDebug,
                          1);
         BXVD_Reg_Write32(hXvd, 
                          hXvd->stPlatformInfo.stReg.uiDecode_BaseCPUAux+BXVD_P_ARC_STATUS32,
                          1); 
      }
   }

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
                      "Backend" );

   BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_SWReset, 
                      hXvd->stPlatformInfo.stReg.uiDecode_SWReset_ILSIResetMask,
                      "ILSI" );

   if (hXvd->bSVCCapable)
   {
      BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_BLD_MainCtl,
                         hXvd->stPlatformInfo.stReg.uiDecode_BLD_MainCtl_ResetMask,
                         "BLD Backend" );

      BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_BLD_ILSScale,
                         hXvd->stPlatformInfo.stReg.uiDecode_BLD_ILSScale_ResetMask,
                         "BLD Scaler" );

      BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_BLD_SintStrmSts,
                         hXvd->stPlatformInfo.stReg.uiDecode_BLD_SintStrmSts_ResetMask,
                         "BLD Stream" );

      BXVD_P_RESET_CORE( hXvd, hXvd->stPlatformInfo.stReg.uiDecode_BLD_SWReset,
                         hXvd->stPlatformInfo.stReg.uiDecode_BLD_SWReset_ILSIResetMask,
                         "BLD ILSI" );
   }

#if BXVD_P_SVD_GISB_ERR_WORKAROUND
      
   BREG_Write32(hXvd->hReg, 
                hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrl, 
                hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrlMask);

   BREG_Write32(hXvd->hReg,          
                hXvd->stPlatformInfo.stReg.uiSun_SWInitSet, 
                hXvd->stPlatformInfo.stReg.uiSun_SWInitSetAvdMask);

   BREG_Write32(hXvd->hReg, 
                hXvd->stPlatformInfo.stReg.uiSun_SWInitClear, 
                hXvd->stPlatformInfo.stReg.uiSun_SWInitClearAvdMask);

   BREG_Write32(hXvd->hReg, hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrl, 0);

   /* Clear interrupt mask (Enable) register to prevent interrupts before ISR is setup */
   BREG_Write32(hXvd->hReg, 
                hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptMask,
                0);

   if (hXvd->bSVCCapable )
   {
      /* SW7425-628: work around for the bus error caused by register reads. */

      uint32_t uiValue;

      /* Enable the RBUS-GISB-RBUS Bridge interrupt. */
      uiValue = BREG_Read32(hXvd->hReg, BCHP_SVD_RGR_0_CTRL);
      uiValue |= BCHP_SVD_RGR_0_CTRL_gisb_error_intr_MASK;

      BREG_Write32(hXvd->hReg, BCHP_SVD_RGR_0_CTRL, uiValue);

      /* Mask the RBUS-GISB-RBUS Bridge interrupt.  */
      BREG_Write32(hXvd->hReg, 
                   BCHP_SVD_INTR2_0_CPU_MASK_SET, 
                   BCHP_SVD_INTR2_0_CPU_MASK_SET_AVD_RGR_BRIDGE_INTR_MASK);
   }
#else

   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrl, 
                    hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrlMask);

   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiSun_SWInitSet, 
                    hXvd->stPlatformInfo.stReg.uiSun_SWInitSetAvdMask);

   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiSun_SWInitClear, 
                    hXvd->stPlatformInfo.stReg.uiSun_SWInitClearAvdMask);

   BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiAvd_SoftShutdownCtrl, 0);

   /* Clear interrupt mask (Enable) register to prevent interrupts before ISR is setup */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptMask,
                    0);
#endif

#if (!BXVD_P_DVD_CHIP)

   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_StripeWidth,
                    hXvd->uiDecode_SDStripeWidthRegVal);

   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_PFRIData,
                    hXvd->uiDecode_PFRIDataRegVal);

   if (hXvd->bSVCCapable)
   {
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_BLStripeWidth,
                       hXvd->uiDecode_SDStripeWidthRegVal);

      BXVD_Reg_Write32(hXvd, 
                          hXvd->stPlatformInfo.stReg.uiDecode_BLPFRIData,
                          hXvd->uiDecode_PFRIDataRegVal);
   }
#endif

   /* Write the AVD PCache mode to hardware */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiAVD_PcacheMode,
                    hXvd->uiAVD_PCacheRegVal);

   BDBG_LEAVE(BXVD_P_ChipReset_RevK0);

   return BERR_TRACE(BERR_SUCCESS);
}
#endif


#if BXVD_P_USE_FW_LOAD_CORE_REVK0
BERR_Code BXVD_P_FWLoad_RevK0(BXVD_Handle hXvd, 
                              uint32_t uiDecoderInstance)
{
   uint32_t uiEndOfCode;
   uint32_t uiILSize; 
   void *pvCmdBuffer;
   void *pvCachedCmdBuffer;

   uint32_t uiDestVirtAddr;
   void *pvCachedDestVA;

   bool bLoadDataOnly;

   BAFL_FirmwareLoadInfo stBAFLoadInfo;

   BXVD_IMAGE_ContextEntry *pOuterImgContext;
   BXVD_IMAGE_ContextEntry *pInnerImgContext;

#if BXVD_P_SVD_PRESENT
   BXVD_IMAGE_ContextEntry *pBaseImgContext;
#endif

   BERR_Code rc;

   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(hXvd->stSettings.pImgInterface);
   BDBG_ASSERT(hXvd->stSettings.pImgContext);
   BSTD_UNUSED(uiDecoderInstance);

   BDBG_ENTER(BXVD_P_FWLoad_RevK0);

   BXVD_DBG_MSG(hXvd, ("Loading Outer Loop ELF image"));

   /* Get image context entry for Outer loop ARC */
   pOuterImgContext = ((BXVD_IMAGE_ContextEntry **)hXvd->stSettings.pImgContext)[BXVD_IMAGE_RevK_FirmwareID_eOuterELF_AVD];
   pInnerImgContext = ((BXVD_IMAGE_ContextEntry **)hXvd->stSettings.pImgContext)[BXVD_IMAGE_RevK_FirmwareID_eInnerELF_AVD];

   if (hXvd->eAVDBootMode == BXVD_AVDBootMode_eNormal)
   {
      bLoadDataOnly = false;
   }  
   else
   {
      bLoadDataOnly = true;
   }  

   if (hXvd->stSettings.pAVDBootCallback && (bLoadDataOnly == false))
   {
      /* If boot call back set and loading code, zero out FW image.  */
      BKNI_Memset((void*)hXvd->uiFWMemVirtAddr, 0, BXVD_P_FW_IMAGE_SIZE);
   }

   rc = BMEM_Heap_ConvertAddressToCached(hXvd->hCodeHeap,(void *) hXvd->uiFWMemVirtAddr, &pvCachedDestVA);

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   rc = BAFL_Load ( hXvd->stSettings.pImgInterface,
                    hXvd->stSettings.pImgContext,
                    BXVD_IMAGE_FirmwareID_eOuterELF_AVD0, 
                    pvCachedDestVA, 
                    BXVD_P_FW_INNER_IMAGE_OFFSET,  /* Size of Outer Loop FW memory is start of Inner Loop FW */ 
                    bLoadDataOnly, 
                    &stBAFLoadInfo );

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   rc = BMEM_Heap_FlushCache(hXvd->hCodeHeap, pvCachedDestVA, BXVD_P_FW_INNER_IMAGE_OFFSET);

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   hXvd->astFWBootInfo[0].uiArcInstance = BXVD_IMAGE_FirmwareID_eOuterELF_AVD0; 
   hXvd->astFWBootInfo[0].stCode.pStartAddress = stBAFLoadInfo.stCode.pStartAddress;
   hXvd->astFWBootInfo[0].stCode.uiSize = stBAFLoadInfo.stCode.uiSize;
       
   hXvd->uiOuterLoopInstructionBase = hXvd->uiFWMemPhyAddr; 

   uiEndOfCode = hXvd->uiOuterLoopInstructionBase + stBAFLoadInfo.stCode.uiSize + stBAFLoadInfo.stData.uiSize;
      
   hXvd->uiOuterLoopEOC = uiEndOfCode -  hXvd->uiOuterLoopInstructionBase; 

   hXvd->uiCmdBufferVector = hXvd->uiOuterLoopInstructionBase + stBAFLoadInfo.stCode.uiSize;

   BXVD_DBG_MSG(hXvd, ("OL InstrBase: %08x", hXvd->uiOuterLoopInstructionBase));
   BXVD_DBG_MSG(hXvd, ("OL Start of code addr: %0x",    hXvd->uiOuterLoopInstructionBase));

   BXVD_DBG_MSG(hXvd, ("OL Code Size: %08x\n",  stBAFLoadInfo.stCode.uiSize));
   BXVD_DBG_MSG(hXvd, ("OL Data Size: %08x\n",  stBAFLoadInfo.stData.uiSize));
   BXVD_DBG_MSG(hXvd, ("OL End of code: %x", hXvd->uiOuterLoopEOC));

   BXVD_DBG_MSG(hXvd, ("Loading Inner Loop ELF image"));  

   uiDestVirtAddr = hXvd->uiFWMemVirtAddr + BXVD_P_FW_INNER_IMAGE_OFFSET;

   BXVD_DBG_MSG(hXvd,  ("Inner Loop FWBase: %08x, Offset: %08x, DestAddr: %08x",       
                        hXvd->uiFWMemVirtAddr, BXVD_P_FW_INNER_IMAGE_OFFSET, uiDestVirtAddr ));

#if BXVD_P_SVD_PRESENT
   /* If SVD capable, setup to load BLD ARC */
   if (hXvd->bSVCCapable)
   {
      /* Base Layer image offset minus Inner image offset */
      pBaseImgContext = ((BXVD_IMAGE_ContextEntry **)hXvd->stSettings.pImgContext)[BXVD_IMAGE_RevK_FirmwareID_eBaseELF_SVD];

      uiILSize = BXVD_P_FW_BASELAYER_IMAGE_OFFSET - BXVD_P_FW_INNER_IMAGE_OFFSET;
   }
   else
   {
      uiILSize = BXVD_P_FW_IMAGE_SIZE - BXVD_P_FW_INNER_IMAGE_OFFSET;
   }
#else
   uiILSize = BXVD_P_FW_IMAGE_SIZE - BXVD_P_FW_INNER_IMAGE_OFFSET;
#endif
   
   rc = BMEM_Heap_ConvertAddressToCached(hXvd->hCodeHeap, (void *) uiDestVirtAddr, &pvCachedDestVA);

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   rc = BAFL_Load ( hXvd->stSettings.pImgInterface,
                    hXvd->stSettings.pImgContext,
                    BXVD_IMAGE_FirmwareID_eInnerELF_AVD0,
                    pvCachedDestVA, 
                    uiILSize,
                    bLoadDataOnly, 
                    &stBAFLoadInfo );

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   rc = BMEM_Heap_FlushCache(hXvd->hCodeHeap, pvCachedDestVA, uiILSize);

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   hXvd->astFWBootInfo[1].uiArcInstance=1;
   hXvd->astFWBootInfo[1].stCode.pStartAddress = stBAFLoadInfo.stCode.pStartAddress;
   hXvd->astFWBootInfo[1].stCode.uiSize = stBAFLoadInfo.stCode.uiSize;

   hXvd->uiInnerLoopInstructionBase = hXvd->uiFWMemPhyAddr + BXVD_P_FW_INNER_IMAGE_OFFSET;
   uiEndOfCode = hXvd->uiInnerLoopInstructionBase + stBAFLoadInfo.stCode.uiSize + stBAFLoadInfo.stData.uiSize;

   BXVD_DBG_MSG(hXvd, ("IL Start of Code: %08x\n", hXvd->uiInnerLoopInstructionBase));
   BXVD_DBG_MSG(hXvd, ("IL Code Size: %08x\n", stBAFLoadInfo.stCode.uiSize));
   BXVD_DBG_MSG(hXvd, ("IL Data Size: %08x\n", stBAFLoadInfo.stData.uiSize));
   BXVD_DBG_MSG(hXvd, ("IL End of Code:: %08x\n",  uiEndOfCode));

   hXvd->uiInnerLoopEOC = uiEndOfCode - hXvd->uiInnerLoopInstructionBase;

   BXVD_DBG_MSG(hXvd, ("InnerLoopInstBase: %08x", hXvd->uiInnerLoopInstructionBase));
   BXVD_DBG_MSG(hXvd, ("End of inner loop code at %x", hXvd->uiInnerLoopEOC));

   BMEM_ConvertOffsetToAddress(hXvd->hCodeHeap, 
                               (uint32_t)hXvd->uiCmdBufferVector, 
                               &pvCmdBuffer); 

   rc = BMEM_Heap_ConvertAddressToCached(hXvd->hCodeHeap, (void *) pvCmdBuffer, &pvCachedCmdBuffer);

   if (rc != BERR_SUCCESS) 
   {
      return BERR_TRACE(rc);
   }

   hXvd->stDecoderContext.ulCmdBufferAddr = (uint32_t) pvCachedCmdBuffer;
   hXvd->stDecoderContext.ulRspBufferAddr = (uint32_t) pvCachedCmdBuffer;

#if BXVD_P_SVD_PRESENT
   /* Check to see if BL ARC is present in this decoder */ 
   if (hXvd->bSVCCapable)
   {
      pBaseImgContext = ((BXVD_IMAGE_ContextEntry **)hXvd->stSettings.pImgContext)[BXVD_IMAGE_RevK_FirmwareID_eBaseELF_SVD];

      uiDestVirtAddr = hXvd->uiFWMemVirtAddr + BXVD_P_FW_BASELAYER_IMAGE_OFFSET;
      BXVD_DBG_MSG(hXvd,  ("Base Layer FWBase: %08x, Offset: %08x, DestAddr: %08x",       
                           hXvd->uiFWMemVirtAddr, BXVD_P_FW_BASELAYER_IMAGE_OFFSET, uiDestVirtAddr ));

      rc = BMEM_Heap_ConvertAddressToCached(hXvd->hCodeHeap, (void *) uiDestVirtAddr, &pvCachedDestVA);

      if (rc != BERR_SUCCESS) 
      {
         return BERR_TRACE(rc);
      }

      rc = BAFL_Load ( hXvd->stSettings.pImgInterface,
                       hXvd->stSettings.pImgContext,
                       BXVD_IMAGE_RevK_FirmwareID_eBaseELF_SVD,
                       pvCachedDestVA, 
                       BXVD_P_FW_IMAGE_SIZE - BXVD_P_FW_BASELAYER_IMAGE_OFFSET, /* Size of memory space where FW is loaded */
                       bLoadDataOnly,
                       &stBAFLoadInfo );

      if (rc != BERR_SUCCESS) 
      {
         return BERR_TRACE(rc);
      }

      rc = BMEM_Heap_FlushCache(hXvd->hCodeHeap, pvCachedDestVA, BXVD_P_FW_IMAGE_SIZE - BXVD_P_FW_BASELAYER_IMAGE_OFFSET);

      if (rc != BERR_SUCCESS) 
      {
         return BERR_TRACE(rc);
      }

      hXvd->astFWBootInfo[2].uiArcInstance = BXVD_IMAGE_RevK_FirmwareID_eBaseELF_SVD;
      hXvd->astFWBootInfo[2].stCode.pStartAddress = stBAFLoadInfo.stCode.pStartAddress;
      hXvd->astFWBootInfo[2].stCode.uiSize = stBAFLoadInfo.stCode.uiSize;

      hXvd->uiBaseInstructionBase = hXvd->uiFWMemPhyAddr + BXVD_P_FW_BASELAYER_IMAGE_OFFSET;
      uiEndOfCode = hXvd->uiBaseInstructionBase + stBAFLoadInfo.stCode.uiSize + stBAFLoadInfo.stData.uiSize;

      BXVD_DBG_MSG(hXvd, ("BL Start of Code: %08x\n", hXvd->uiBaseInstructionBase));
      BXVD_DBG_MSG(hXvd, ("BL Code Size: %08x\n", stBAFLoadInfo.stCode.uiSize));
      BXVD_DBG_MSG(hXvd, ("BL Data Size: %08x\n", stBAFLoadInfo.stData.uiSize));
      BXVD_DBG_MSG(hXvd, ("BL End of Code:: %08x\n", uiEndOfCode));

      hXvd->uiBaseEOC = uiEndOfCode - hXvd->uiBaseInstructionBase;

      BXVD_DBG_MSG(hXvd, ("BaseLayerInstBase: %08x", hXvd->uiBaseInstructionBase));
      BXVD_DBG_MSG(hXvd, ("End of Base Layer code at %x", hXvd->uiBaseEOC));

   }
#endif

   BDBG_LEAVE(BXVD_P_FWLoad_RevK0);

   return BERR_TRACE(BERR_SUCCESS);
}
#endif /* BXVD_P_USE_FW_LOAD_CORE_REVK0 */

#if BXVD_P_USE_CORE_CHIP_ENABLE_REVK0

BERR_Code BXVD_P_ChipEnable_RevK0(BXVD_Handle hXvd)
{
   volatile uint32_t uiFWBootStatus;

   BAFL_BootInfo stAVDBootInfo;

   BAFL_FirmwareInfo astFirmwareInfo[3];

   BERR_Code rc = BERR_SUCCESS;

#if BXVD_POLL_FW_MBX
   uint32_t uiVal;
   int loopCount;
#endif

   BDBG_ASSERT(hXvd);
   BDBG_ENTER(BXVD_P_ChipEnable_RevK0);

   /* 
    * Write to VectorTB, CpuDbg registers and AuxRegs 
    * in THIS ORDER to start Outer Loop ARC  
    */

   BDBG_MSG(("OL Ibase: %0x", hXvd->uiOuterLoopInstructionBase));
   BDBG_MSG(("OL EOC: %0x", hXvd->uiOuterLoopEOC));

   BDBG_MSG(("IL Ibase: %0x",hXvd->uiInnerLoopInstructionBase));
   BDBG_MSG(("IL EOC: %0x", hXvd->uiInnerLoopEOC));

   /* Program the relocation base address for outer-loop */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterInstructionBase,
                    hXvd->uiOuterLoopInstructionBase); 

   /* Program the outer loop image end of code address */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterEndOfCode, 
                    hXvd->uiOuterLoopEOC);
        
   /* Program the relocation base address for inner-loop */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerInstructionBase,
                    hXvd->uiInnerLoopInstructionBase); 
        
   /* Program the inner loop image end of code address */
   BXVD_Reg_Write32(hXvd, 
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerEndOfCode, 
                    hXvd->uiInnerLoopEOC);
        
   /* Program global IO base register - Outer */
   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterGlobalIOBase,
                    BCHP_PHYSICAL_OFFSET);

   /* Program global IO base register - Inner */
   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiDecode_InnerGlobalIOBase,
                    BCHP_PHYSICAL_OFFSET);          

#if BXVD_P_SVD_PRESENT
   if (hXvd->bSVCCapable)
   {

      BDBG_MSG(("BL Ibase: %0x",hXvd->uiBaseInstructionBase));
      BDBG_MSG(("BL EOC: %0x", hXvd->uiBaseEOC));

      /* Program the base layer relocation base address. */
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_BaseInstructionBase,
                       hXvd->uiBaseInstructionBase); 
        
      /* Program the base layer image end of code address */
      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_BaseEndOfCode, 
                       hXvd->uiBaseEOC);
        
      /* Program base layer global IO base register */
      BXVD_Reg_Write32(hXvd,
                       hXvd->stPlatformInfo.stReg.uiDecode_BaseGlobalIOBase,
                       BCHP_PHYSICAL_OFFSET);
      
   }
#endif

   /* Clear out any previously completed FW command done events */
   BKNI_ResetEvent(hXvd->stDecoderContext.hFWCmdDoneEvent);

   BXVD_Reg_Write32(hXvd,
                    hXvd->stPlatformInfo.stReg.uiDecode_OuterHost2CPUMailbox,
                    hXvd->uiAVDCoreFreq);

#if BXVD_POLL_FW_MBX 
   /* Initialize MBX to non-zero */
   BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox, 0xff);
   uiVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox);
   BDBG_MSG(("Initial CPU2HostMB: %0x\n", uiVal));
#endif
        
   if (hXvd->stSettings.pAVDBootCallback)
   {
      /* Set boot mode */
      if ( hXvd->eAVDBootMode == BXVD_AVDBootMode_eNormal)
      {
         stAVDBootInfo.eMode = BAFL_BootMode_eNormal;
      }
      else
      {
         stAVDBootInfo.eMode = BAFL_BootMode_eWatchdog;
      }

      stAVDBootInfo.pstArc = &astFirmwareInfo[0];

      /* Set Outer Loop ARC firmware info */
      astFirmwareInfo[0].uiArcInstance = 0;
      astFirmwareInfo[0].stCode.pStartAddress = hXvd->astFWBootInfo[0].stCode.pStartAddress;
      astFirmwareInfo[0].stCode.uiSize = hXvd->astFWBootInfo[0].stCode.uiSize;
      astFirmwareInfo[0].pNext = &astFirmwareInfo[1];

      /* Set Inner Loop ARC firmware info */
      astFirmwareInfo[1].uiArcInstance = 1;
      astFirmwareInfo[1].stCode.pStartAddress = hXvd->astFWBootInfo[1].stCode.pStartAddress;
      astFirmwareInfo[1].stCode.uiSize = hXvd->astFWBootInfo[1].stCode.uiSize;
      astFirmwareInfo[1].pNext = NULL;

#if BXVD_P_SVD_PRESENT
      if (hXvd->bSVCCapable)
      {
         astFirmwareInfo[1].pNext = &astFirmwareInfo[2];
 
         /* Set Base Layer ARC firmware info */
         astFirmwareInfo[2].uiArcInstance = 2;
         astFirmwareInfo[2].stCode.pStartAddress = hXvd->astFWBootInfo[2].stCode.pStartAddress;
         astFirmwareInfo[2].stCode.uiSize = hXvd->astFWBootInfo[2].stCode.uiSize;
         astFirmwareInfo[2].pNext = NULL;      
      }
#endif
      /* Call external boot callback */
      rc = (*hXvd->stSettings.pAVDBootCallback)(hXvd->stSettings.pAVDBootCallbackData,
                                                &stAVDBootInfo);
      if (rc != BERR_SUCCESS) 
      {
         BXVD_DBG_ERR(hXvd, ("Error #%d booting the AVD externally", rc));
         return BERR_TRACE(rc);
      }
   } 
   else 
   {
      /* Boot the core */

      BXVD_Reg_Write32(hXvd, 
                       hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug,
                       1);

      /* Write ARC PC start address */
      BXVD_Reg_Write32(hXvd, 
                       (hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux+BXVD_P_ARC_PC),
                       0); 

      /* Start Arc */
      BXVD_Reg_Write32(hXvd,
                       (hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux+BXVD_P_ARC_STATUS32),
                       0);
   }
         
#define FW_CMD_TIMEOUT 10000

#if !BXVD_POLL_FW_MBX 

   rc = BERR_TRACE(BKNI_WaitForEvent(hXvd->stDecoderContext.hFWCmdDoneEvent, FW_CMD_TIMEOUT));

#else

   uiVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox);

   loopCount = 0;
   rc = BERR_TIMEOUT;

   while (loopCount < FW_CMD_TIMEOUT)
   {
      if (uiVal != 0)
      {
         uiFWBootStatus = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostStatus);

         BDBG_MSG(("ARC FW Boot Status = %d", uiFWBootStatus));

         BDBG_MSG(("loopCount:%d Calling BKNI_Sleep(1), MBX:%d\n", loopCount, uiVal));
         BKNI_Sleep(1);  

         loopCount++;
         uiVal = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostMailbox);
      }
      else 
      {
         rc = BERR_SUCCESS;
         break;
      }
   }

#endif

   /* Read FW boot progress/status from CPU2HostStatus register that was written by FW */
   uiFWBootStatus = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPU2HostStatus);

   if(BERR_TIMEOUT == rc)
   {
      BXVD_DBG_ERR(hXvd, ("ARC FW command response timed out, FW Boot Status = %d", uiFWBootStatus));

      return BERR_TRACE(rc);
   }
   else
   {
      BXVD_DBG_MSG(hXvd, ("FW boot successful, FW Boot Status = %d", uiFWBootStatus));
   }

   BKNI_ResetEvent(hXvd->stDecoderContext.hFWCmdDoneEvent);
            
   BDBG_LEAVE(BXVD_P_ChipEnable_RevK0);
   return BERR_TRACE(rc);
}

#endif /* BXVD_P_USE_CORE_CHIP_ENABLE_REVK0 */

#if BXVD_P_USE_READ_DISPLAY_INFO_HIM_API

void BXVD_P_ReadDisplayInfo_HIM_API(BXVD_Handle hXvd, 
                                    uint32_t uiDisplayInfoOffset,
                                    BXVD_P_DisplayInfo *pstDisplayInfo)
{ 
   uint32_t i, wordOffset; 
   
   wordOffset = uiDisplayInfoOffset;

   for (i = 0; i < BXVD_P_STC_MAX; i++)
   {
      BXVD_P_READ_HIM(hXvd, wordOffset, pstDisplayInfo->stc_snapshot[i]);
      wordOffset++;
   } 

   BXVD_P_READ_HIM(hXvd, wordOffset, pstDisplayInfo->vsync_parity);
}

bool BXVD_P_IsDisplayInfoEqual_HIM_API(BXVD_P_DisplayInfo stDisplayInfo, 
                                       BXVD_P_DisplayInfo stDisplayInfo1)
{
   uint32_t i; 
   bool bDisplayInfoEqual = true;

   for (i=0; i<8; i++)
   {
      if (stDisplayInfo.stc_snapshot[i] != stDisplayInfo1.stc_snapshot[i])
      {
         bDisplayInfoEqual = false;
         break;
      }
   }

   if (stDisplayInfo.vsync_parity != stDisplayInfo1.vsync_parity)
   {
      bDisplayInfoEqual = false;
   }

   return bDisplayInfoEqual;
}

#endif  /* BXVD_P_USE_READ_DISPLAY_INFO_HIM_API */


#if BXVD_P_USE_SET_POWER_STATE_REVK0 

void BXVD_P_SetPowerState_RevK0(BXVD_Handle hXvd,
                                BXVD_P_PowerState PowerStateNew)
{
   BCHP_PWR_ResourceId clkResourceId = BCHP_PWR_RESOURCE_AVD0_CLK;
   BCHP_PWR_ResourceId pwrResourceId = BCHP_PWR_RESOURCE_AVD0_PWR;

#if BCHP_PWR_RESOURCE_AVD1
   if (hXvd->uDecoderInstance == 1)
   {
      clkResourceId = BCHP_PWR_RESOURCE_AVD1_CLK;
      pwrResourceId = BCHP_PWR_RESOURCE_AVD1_PWR;
   }
#endif

   BDBG_MSG(("BXVD_P_SetPowerState, Decoder:%d Cur: %d New: %d", 
             hXvd->uDecoderInstance, hXvd->PowerStateCurrent , PowerStateNew));

   if (hXvd->PowerStateCurrent != PowerStateNew)
   {
      if ((hXvd->PowerStateCurrent == BXVD_P_PowerState_eOn) &&
          ((PowerStateNew == BXVD_P_PowerState_eClkOff) ||
           (PowerStateNew == BXVD_P_PowerState_ePwrOff)))
      {
         BDBG_MSG(("Release AVD_Clk Resource"));
         BCHP_PWR_ReleaseResource(hXvd->hChip, clkResourceId);   
      }

      if (((hXvd->PowerStateCurrent == BXVD_P_PowerState_eOn) ||
           (hXvd->PowerStateCurrent == BXVD_P_PowerState_eClkOff)) &&
          (PowerStateNew == BXVD_P_PowerState_ePwrOff))
      {
         BDBG_MSG(("Release AVD_PWR Resource"));
         BCHP_PWR_ReleaseResource(hXvd->hChip, pwrResourceId);   
      }

      if ((hXvd->PowerStateCurrent == BXVD_P_PowerState_ePwrOff) &&
          ((PowerStateNew == BXVD_P_PowerState_eClkOff) ||
           (PowerStateNew == BXVD_P_PowerState_eOn)))
      {
         BDBG_MSG(("Acquire AVD_PWR Resource\n"));

         BCHP_PWR_AcquireResource(hXvd->hChip, pwrResourceId);   
      }

      if (((hXvd->PowerStateCurrent == BXVD_P_PowerState_eClkOff) ||
           (hXvd->PowerStateCurrent == BXVD_P_PowerState_ePwrOff)) &&
          (PowerStateNew == BXVD_P_PowerState_eOn))
      {
         BDBG_MSG(("Acquire AVD_CLK Resource"));
         BCHP_PWR_AcquireResource(hXvd->hChip, clkResourceId);   
      }

      hXvd->PowerStateCurrent = PowerStateNew;
   }
}

#endif /* BXVD_P_USE_SET_POWER_STATE_REVK0 */

#ifdef BXVD_P_USE_VERIFY_WATCHDOG_FIRED_REVK0 

bool BXVD_P_VerifyWatchdogFired_RevK0(BXVD_Handle hXvd,
                                      int param2)
{
   volatile uint32_t     uIntrVal;

   volatile uint32_t     uiOL_pc;
   int i;
   
   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_P_VerifyWatchdogFired_RevK0);
   if ( param2 == BXVD_P_OUTER_WATCHDOG)
   {
      uIntrVal = BXVD_Reg_Read32(hXvd, 
                                 hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptClear);

      BXVD_DBG_MSG(hXvd, ("L3IntClr:%08x", uIntrVal));

      if ( uIntrVal & BXVD_P_INTR_OL_MASK)
      {
         BXVD_DBG_MSG(hXvd, ("Outer Loop Watchdog"));
         BXVD_DBG_ERR(hXvd, ("OL watchdog - firmware revision: %x", hXvd->sRevisionInfo.ulDecoderFwRev));

         BXVD_Reg_Write32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUDebug, 1);

         /* Read FW OL PC to help with debugging watchdog timeouts */
         for (i = 0; i < 8; i++)
         {
            /* read the AVD OL PC */
            uiOL_pc = BXVD_Reg_Read32(hXvd, hXvd->stPlatformInfo.stReg.uiDecode_OuterCPUAux+BXVD_P_ARC_PC);

            BXVD_DBG_ERR(hXvd, ("[%d] AVD_%d: OL PC=%08x", i, hXvd->uDecoderInstance, uiOL_pc));
         }

         BXVD_Reg_Write32(hXvd,
                          hXvd->stPlatformInfo.stReg.uiDecode_OuterInterruptClear,
                          uIntrVal);

         BXVD_DBG_MSG(hXvd, ("Clear AVD0_INTR2_L3 intr"));
         /* Clear L2 interrupt, this may be necessary */
         BXVD_Reg_Write32(hXvd,
                          hXvd->stPlatformInfo.stReg.uiAvd_CPUL2InterruptClear,
                          hXvd->stPlatformInfo.stReg.uiAvd_CPUL2InterruptMailboxMask);
      }

      if (!(uIntrVal & BXVD_P_INTR_OL_MASK))
      {
         return false;
      }
   }
   else
   {
      BXVD_DBG_MSG(hXvd, ("Inner Loop Watchdog"));
   }

   BDBG_LEAVE(BXVD_P_VerifyWatchdogFired_RevK0);

   return true;
}

#endif /*  BXVD_P_USE_VERIFY_WATCHDOG_FIRED_REVK0 */
