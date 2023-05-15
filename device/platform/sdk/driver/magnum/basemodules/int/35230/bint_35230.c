/***************************************************************************
 *       Copyright (c) 2003-2011, Broadcom Corporation
 *       All Rights Reserved
 *       Confidential Property of Broadcom Corporation
 *
 *    THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *    AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *    EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_35230.c $
 * $brcm_Revision: Hydra_Software_Devel/33 $
 * $brcm_Date: 7/14/11 9:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/35230/bint_35230.c $
 * 
 * Hydra_Software_Devel/33   7/14/11 9:53p jhaberf
 * SWDTV-7838: merge of 35123 c1 power management changes into the main
 * 
 * Hydra_Software_Devel/32   3/23/11 3:22p jhaberf
 * SWDTV-6170: Changed BINT mapping for SmartCard
 * 
 * Hydra_Software_Devel/31   3/15/11 12:53p jhaberf
 * SW35330-13: added 35233 DTV chip support
 * 
 * Hydra_Software_Devel/30   2/22/11 7:20p jhaberf
 * SW35233-13: Added 35233 DTV chip support
 * 
 * Hydra_Software_Devel/29   12/1/10 8:01p jhaberf
 * SW35125-1: resolved link error for the 35125
 * 
 * Hydra_Software_Devel/28   11/5/10 8:39p jhaberf
 * SW35125-1: Added 35125 support to BINT
 * 
 * Hydra_Software_Devel/27   10/22/10 4:31p jhaberf
 * SW35230-1833: Removed some duplicated interrupts
 * 
 * Hydra_Software_Devel/26   10/19/10 1:36p jhaberf
 * SW35230-1833: Added additional XPT interrupts for playpump
 * 
 * Hydra_Software_Devel/25   9/27/10 2:53p jhaberf
 * SW35230-1498: Updated module for the 35230 C0 register set
 * 
 * Hydra_Software_Devel/24   8/31/10 4:30p jhaberf
 * SW35230-267: Removed TVM2MAIN_COMM_CPU_INTR from the interrupts handled
 * by bcmdriver/BINT as this interrupt is being handled in the kernel
 * 
 * Hydra_Software_Devel/23   7/20/10 6:15p jhaberf
 * SW35230-1: Removed a number #ifdefs for chip ID which were not
 * applicable as the file is for the 35230 only
 * 
 * Hydra_Software_Devel/22   6/28/10 10:44a jhaberf
 * SW35230-267: masked on PCU DMA interrupt used by kernel
 * 
 * Hydra_Software_Devel/21   6/28/10 12:25p hauxwell
 * SW35230-453: Change V3D IRQ mapping to passthrough
 * 
 * Hydra_Software_Devel/20   6/24/10 3:56p jhaberf
 * SW35230-267: Reverted sharing of PCU DMA interrupt as this caused
 * problems with DVB-CI
 * 
 * Hydra_Software_Devel/19   6/23/10 8:56p jhaberf
 * SW35230-267: Changed status of PCU DMA interrupt to shared as this
 * interrupt is needed by both the kernel and the SPI PI
 * 
 * Hydra_Software_Devel/18   6/5/10 12:00p jhaberf
 * SW35230-267: Added interrupt mapping for PCU_UIRT1_CPU_INTR L2 register
 * 
 * Hydra_Software_Devel/17   6/4/10 8:06a jhaberf
 * SW35230-1: Updating bint at the request of the TFW team
 * 
 * Hydra_Software_Devel/16   5/30/10 1:30p jhaberf
 * SW35230-1: Mapped the HIF_CPU_INTR L1 register
 * 
 * Hydra_Software_Devel/15   5/28/10 7:20p jhaberf
 * SW35230-1: Corrected the interrupt mapping for GPIO
 * 
 * Hydra_Software_Devel/14   5/28/10 5:44p jhaberf
 * SW35230-1: Unmapped TVM micro UART interrupt as this is currently being
 * used by the kernel
 * 
 * Hydra_Software_Devel/13   5/26/10 5:39p jhaberf
 * SW35230-1: Updated TFE interrupt handling
 * 
 * Hydra_Software_Devel/12   5/22/10 8:57p jhaberf
 * SW35230-1: Removed handling of HIF_CPU_INTR interrupt as this interrupt
 * shouldn't be handled by BINT
 * 
 * Hydra_Software_Devel/11   5/22/10 8:31p jhaberf
 * SW35230-1: Removing interrupts handled by kernel
 * 
 * Hydra_Software_Devel/10   5/22/10 12:32p jhaberf
 * SW35230-1: Removed space from end of line as this was causing problems
 * with the no-os environment
 * 
 * Hydra_Software_Devel/9   5/19/10 3:14p jhaberf
 * SW35230-1: Added BCHP_V3D_CTL_INTCTL to the standard L2 cases
 * 
 * Hydra_Software_Devel/8   5/14/10 6:39p jhaberf
 * SW35230-1: Added L2 interrupt register for V3D
 * 
 * Hydra_Software_Devel/7   1/21/10 11:17a paultsai
 * SW35230-1: Update BINT_P_XPT_STATUS_CASES and temporarily comment out
 * incomplete interrupt mappings to avoid assertions in debug mode
 * 
 * Hydra_Software_Devel/6   12/4/09 4:41p jhaberf
 * SW35230-1: Added offset for W1/W2 interrupt status registers
 * 
 * Hydra_Software_Devel/5   11/2/09 2:06p jhaberf
 * SW35230-1: Updated interrupt handling to match latest RDB
 * 
 * Hydra_Software_Devel/4   10/30/09 1:28p jhaberf
 * SW35230-1: Updated file for 26-Oct-09 register changes
 * 
 * Hydra_Software_Devel/3   10/6/09 3:47p jhaberf
 * SW35230-1: Added additional IRQ support for 35230
 * 
 * Hydra_Software_Devel/2   10/2/09 7:06p jhaberf
 * SW35230-1: Updated interrupt mappings for 35230 for
 * HIF_CPU_INTR1_INTR_W0_STATUS register
 * 
 * Hydra_Software_Devel/1   9/10/09 6:36p jhaberf
 * SW35230-1: Creating 35230 DTV chip build environment
 * 
 * Hydra_Software_Devel/1   9/9/09 4:17p jhaberf
 * SW35230-1: Creating 35230 DTV chip build environment
 *
 ***************************************************************************/
#include "bstd.h"
#if (BCHP_CHIP==35230)
#include "bint_35230.h"
#elif (BCHP_CHIP==35125)
#include "bint_35125.h"
#elif (BCHP_CHIP==35233)
#include "bint_35233.h"
#endif

/* Include interrupt definitions from RDB */
#include "bchp_aio_inth.h"
#include "bchp_hif2pcu_bridge.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_hif_cpu_tp1_intr1.h"
#include "bchp_hifidac_ctrl0.h"
#include "bchp_hifidac_ctrl1.h"
#include "bchp_hifidac_ctrl2.h"
#include "bchp_hifidac_esr0.h"
#include "bchp_hifidac_esr1.h"
#include "bchp_hifidac_esr2.h"
#include "bchp_hifidac_rm0.h"
#include "bchp_hifidac_rm1.h"
#include "bchp_hifidac_rm2.h"
#include "bchp_hif_pci_rg.h"
#include "bchp_hif_rgr.h"
#include "bchp_hif_top_ctrl.h"
#include "bchp_int_id_hif_intr2.h"
#include "bchp_memc_l2_0.h"
#include "bchp_raaga_dsp_inth.h"
#include "bchp_tvm.h"
#include "bchp_adec_core.h"
#include "bchp_dfe_miscdec.h"

#if (BCHP_CHIP == 35230)
#include "bchp_v3d_ctl.h"
#include "bchp_vip_cap1_l2_int.h"
#include "bchp_frc_ctleng_intr2_out.h"
#if (BCHP_VER >= BCHP_VER_C0 )
    #include "bchp_genet_0_intrl2_0.h"
    #include "bchp_genet_0_intrl2_1.h"
#else
    #include "bchp_genet_intrl2_0.h"
    #include "bchp_genet_intrl2_1.h"
#endif/*BCHP_VER*/
#endif/*BCHP_CHIP*/

#include "bchp_vdec_rest.h"
#include "bchp_vip_cap0_l2_int.h"
#include "bchp_vindeco_intr_ctrl2.h"
#include "bchp_pcu.h"
#include "bchp_usb_ohci.h"
#include "bchp_usb_ehci.h"
#include "bchp_usb_intr2.h"
#include "bchp_raaga_dsp_fw_inth.h"
#include "bchp_avs_ctrl.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_l2.h"
#include "bchp_timer.h"
#include "bchp_ipi1_intr2.h"
#include "bchp_ipi0_intr2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_gfx_l2.h"
#include "bchp_hdmi_rx_intr2_0.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_hif_intr2.h"
#include "bchp_irq0.h"
#include "bchp_scirq0.h"
#if (BCHP_CHIP==35230) || (BCHP_CHIP==35125)
#include "bchp_dvpo_intr2_0.h"
#else
/* 35233 header file for DVP goes here */
#endif
#include "bchp_video_enc_intr2.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bchp_xpt_dpcr2.h"
#include "bchp_xpt_dpcr3.h"
#include "bchp_xpt_fe.h"
#include "bchp_xpt_msg.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"
#include "bchp_xpt_pb2.h"
#include "bchp_xpt_pb3.h"
#include "bchp_xpt_pb4.h"
#include "bchp_xpt_rave.h"
#include "bchp_avd_intr2_0.h"
#include "bchp_dvp_hr_intr2.h"
#include "bchp_irq0.h"

BDBG_MODULE(BINT);

/* Standard L2 interrupt cases */
#define BINT_P_STD_STATUS           ( 0x00 )
#define BINT_P_STD_SET              ( 0x04 )
#define BINT_P_STD_CLEAR            ( 0x08 )
#define BINT_P_STD_MASK_STATUS      ( 0x0C )
#define BINT_P_STD_MASK_SET         ( 0x10 )
#define BINT_P_STD_MASK_CLEAR       ( 0x14 )

#if (BCHP_CHIP==35230)
#define BINT_P_STANDARD_L2_CASES \
    case BCHP_DVPO_INTR2_0_CPU_STATUS: \
    case BCHP_MEMC_L2_0_R5F_STATUS: \
    case BCHP_TVM_INT_STATUS: \
    case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
    case BCHP_AVD_INTR2_0_CPU_STATUS: \
    case BCHP_HDMI_RX_INTR2_0_CPU_STATUS: \
    case BCHP_DVP_HR_INTR2_CPU_STATUS: \
    case BCHP_RAAGA_DSP_INTH_HOST_STATUS: \
    case BCHP_AIO_INTH_R5F_STATUS: \
    case BCHP_BVNB_INTR2_CPU_STATUS: \
    case BCHP_BVNF_INTR2_5_R5F_STATUS: \
    case BCHP_BVNF_INTR2_1_R5F_STATUS: \
    case BCHP_BVNF_INTR2_0_R5F_STATUS: \
    case BCHP_GFX_L2_CPU_STATUS: \
    case BCHP_VDEC_REST_VDEC_INTERRUPT_STATUS: \
    case BCHP_VIP_CAP1_L2_INT_CAP1_VIDBLK_CPU_INT_STATUS: \
    case BCHP_VIP_CAP0_L2_INT_CAP0_VIDBLK_CPU_INT_STATUS: \
    case BCHP_VINDECO_INTR_CTRL2_CPU_STATUS: \
    case BCHP_HIF_INTR2_CPU_STATUS: \
    case BCHP_AVS_CTRL_STATUS: \
    case BCHP_USB_OHCI_HcInterruptEnable: \
    case BCHP_USB_EHCI_USBINTR: \
    case BCHP_USB_INTR2_CPU_STATUS: \
    case BCHP_FRC_CTLENG_INTR2_OUT_CPU_STATUS: \
    case BCHP_PCU_PAR_INT_STAT: \
    case BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS: \
    case BCHP_SUN_TOP_CTRL_NMI_CTRL_nmi_pad_monitor_MASK: \
    case BCHP_SCIRQ0_SCIRQSTAT: \
    case BCHP_IRQ0_IRQSTAT: \
    case BCHP_SUN_L2_CPU_STATUS: \
    case BCHP_PCU_UART3_IIR: \
    case BCHP_PCU_UART2_IIR: \
    case BCHP_PCU_UIRT1_INTR_STATUS: \
    case BCHP_PCU_SPI_CTRL: \
    case BCHP_PCU_DMA_INTR_STATUS: \
    case BCHP_PCU_ONENAND_INT_STATUS: \
    case BCHP_IPI1_INTR2_CPU_STATUS: \
    case BCHP_IPI0_INTR2_CPU_STATUS:
#elif (BCHP_CHIP==35125)
#define BINT_P_STANDARD_L2_CASES \
    case BCHP_DVPO_INTR2_0_CPU_STATUS: \
    case BCHP_MEMC_L2_0_R5F_STATUS: \
    case BCHP_TVM_INT_STATUS: \
    case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
    case BCHP_AVD_INTR2_0_CPU_STATUS: \
    case BCHP_HDMI_RX_INTR2_0_CPU_STATUS: \
    case BCHP_DVP_HR_INTR2_CPU_STATUS: \
    case BCHP_RAAGA_DSP_INTH_HOST_STATUS: \
    case BCHP_AIO_INTH_R5F_STATUS: \
    case BCHP_BVNB_INTR2_CPU_STATUS: \
    case BCHP_BVNF_INTR2_5_R5F_STATUS: \
    case BCHP_BVNF_INTR2_1_R5F_STATUS: \
    case BCHP_BVNF_INTR2_0_R5F_STATUS: \
    case BCHP_GFX_L2_CPU_STATUS: \
    case BCHP_VDEC_REST_VDEC_INTERRUPT_STATUS: \
    case BCHP_VIP_CAP0_L2_INT_CAP0_VIDBLK_CPU_INT_STATUS: \
    case BCHP_VINDECO_INTR_CTRL2_CPU_STATUS: \
    case BCHP_HIF_INTR2_CPU_STATUS: \
    case BCHP_AVS_CTRL_STATUS: \
    case BCHP_USB_OHCI_HcInterruptEnable: \
    case BCHP_USB_EHCI_USBINTR: \
    case BCHP_USB_INTR2_CPU_STATUS: \
    case BCHP_PCU_PAR_INT_STAT: \
    case BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS: \
    case BCHP_SUN_TOP_CTRL_NMI_CTRL_nmi_pad_monitor_MASK: \
    case BCHP_SCIRQ0_SCIRQSTAT: \
    case BCHP_IRQ0_IRQSTAT: \
    case BCHP_SUN_L2_CPU_STATUS: \
    case BCHP_PCU_UART3_IIR: \
    case BCHP_PCU_UART2_IIR: \
    case BCHP_PCU_UIRT1_INTR_STATUS: \
    case BCHP_PCU_SPI_CTRL: \
    case BCHP_PCU_DMA_INTR_STATUS: \
    case BCHP_PCU_ONENAND_INT_STATUS: \
    case BCHP_IPI1_INTR2_CPU_STATUS: \
    case BCHP_IPI0_INTR2_CPU_STATUS:
#else
#define BINT_P_STANDARD_L2_CASES \
    case BCHP_MEMC_L2_0_R5F_STATUS: \
    case BCHP_TVM_INT_STATUS: \
    case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
    case BCHP_AVD_INTR2_0_CPU_STATUS: \
    case BCHP_HDMI_RX_INTR2_0_CPU_STATUS: \
    case BCHP_DVP_HR_INTR2_CPU_STATUS: \
    case BCHP_RAAGA_DSP_INTH_HOST_STATUS: \
    case BCHP_AIO_INTH_R5F_STATUS: \
    case BCHP_BVNB_INTR2_CPU_STATUS: \
    case BCHP_BVNF_INTR2_5_R5F_STATUS: \
    case BCHP_BVNF_INTR2_1_R5F_STATUS: \
    case BCHP_BVNF_INTR2_0_R5F_STATUS: \
    case BCHP_GFX_L2_CPU_STATUS: \
    case BCHP_VDEC_REST_VDEC_INTERRUPT_STATUS: \
    case BCHP_VIP_CAP0_L2_INT_CAP0_VIDBLK_CPU_INT_STATUS: \
    case BCHP_VINDECO_INTR_CTRL2_CPU_STATUS: \
    case BCHP_HIF_INTR2_CPU_STATUS: \
    case BCHP_AVS_CTRL_STATUS: \
    case BCHP_USB_OHCI_HcInterruptEnable: \
    case BCHP_USB_EHCI_USBINTR: \
    case BCHP_USB_INTR2_CPU_STATUS: \
    case BCHP_PCU_PAR_INT_STAT: \
    case BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS: \
    case BCHP_SUN_TOP_CTRL_NMI_CTRL_nmi_pad_monitor_MASK: \
    case BCHP_SCIRQ0_SCIRQSTAT: \
    case BCHP_IRQ0_IRQSTAT: \
    case BCHP_SUN_L2_CPU_STATUS: \
    case BCHP_PCU_UART3_IIR: \
    case BCHP_PCU_UART2_IIR: \
    case BCHP_PCU_UIRT1_INTR_STATUS: \
    case BCHP_PCU_SPI_CTRL: \
    case BCHP_PCU_DMA_INTR_STATUS: \
    case BCHP_PCU_ONENAND_INT_STATUS: \
    case BCHP_IPI1_INTR2_CPU_STATUS: \
    case BCHP_IPI0_INTR2_CPU_STATUS:
#endif
	
/* DFE  L2 interrupt cases*/
#define BINT_P_DFE_ADEC_L2_CASES \
    case BCHP_DFE_MISCDEC_MISC_DFE_INT_STATUS: \
    case BCHP_ADEC_CORE_ADEC_INT_STATUS:
    
/* DFE/ADEC interrupt cases */
#define BINT_P_DFE_ADEC_STATUS  (0)
#define BINT_P_DFE_ADEC_ENABLE  (-4)
#define BINT_P_DFE_ADEC_CLEAR   (4)
    
/* Transport L2 interrupt cases */
#define BINT_P_XPT_STATUS           ( 0x00 )
#define BINT_P_XPT_ENABLE           ( 0x04 )

#define BINT_P_XPT_STATUS_CASES \
    case BCHP_XPT_BUS_IF_INTR_STATUS_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS2_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS4_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS5_REG: \
    case BCHP_XPT_FE_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR0_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR1_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR2_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR3_INTR_STATUS_REG: \
    case BCHP_XPT_PB0_INTR: \
    case BCHP_XPT_PB1_INTR: \
    case BCHP_XPT_PB2_INTR: \
    case BCHP_XPT_PB3_INTR: \
    case BCHP_XPT_PB4_INTR:

/* Transport RAVE cases */
#define BINT_P_RAVE_STATUS          ( 0x00 )

#define BINT_P_XPT_RAVE_CASES \
    case BCHP_XPT_RAVE_INT_CX0: \
    case BCHP_XPT_RAVE_INT_CX1: \
    case BCHP_XPT_RAVE_INT_CX2: \
    case BCHP_XPT_RAVE_INT_CX3: \
    case BCHP_XPT_RAVE_INT_CX4: \
    case BCHP_XPT_RAVE_INT_CX5: \
    case BCHP_XPT_RAVE_INT_CX6: \
    case BCHP_XPT_RAVE_INT_CX7: \
    case BCHP_XPT_RAVE_INT_CX8: \
    case BCHP_XPT_RAVE_INT_CX9: \
    case BCHP_XPT_RAVE_INT_CX10: \
    case BCHP_XPT_RAVE_INT_CX11: \
    case BCHP_XPT_RAVE_INT_CX12: \
    case BCHP_XPT_RAVE_INT_CX13: \
    case BCHP_XPT_RAVE_INT_CX14: \
    case BCHP_XPT_RAVE_INT_CX15: \
    case BCHP_XPT_RAVE_INT_CX16: \
    case BCHP_XPT_RAVE_INT_CX17: \
    case BCHP_XPT_RAVE_INT_CX18: \
    case BCHP_XPT_RAVE_INT_CX19: \
    case BCHP_XPT_RAVE_INT_CX20: \
    case BCHP_XPT_RAVE_INT_CX21: \
    case BCHP_XPT_RAVE_INT_CX22: \
    case BCHP_XPT_RAVE_INT_CX23:

/* Transport buffer cases */
#define BINT_P_XPT_BUF_STATUS           ( 0x00 )
#define BINT_P_XPT_BUF_ENABLE           ( 0x10 )

#define BINT_P_XPT_BUF_CASES \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_00_31: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_32_63: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63:

/* Transport message error cases */
#define BINT_P_XPT_MSG_ERR_STATUS       ( 0x00 )
#define BINT_P_XPT_MSG_ERR_ENABLE       ( 0x04 )

#define BINT_P_XPT_MSG_ERR_CASES \
    case BCHP_XPT_MSG_DAT_ERR_INTR:

/* Timer cases */
#define BINT_P_TIMER_STATUS             ( 0x00 )
#define BINT_P_TIMER_ENABLE             ( 0x04 )

#define BINT_P_TIMER_CASES \
    case BCHP_TIMER_TIMER_IS:

/* Smart Card cases */
#define BINT_P_UPGSC_ENABLE             ( 0x00 )
#define BINT_P_UPGSC_STATUS             ( 0x04 )

#define BINT_P_UPGSC_CASES \
    case BCHP_SCIRQ0_SCIRQEN: \


/* IRQ0 cases */
#define BINT_P_IRQ0_ENABLE              ( 0 )
#define BINT_P_IRQ0_STATUS              ( 4 )

/* case BCHP_IRQ0_UARTA_IRQEN : \ */

#define BINT_P_IRQ0_CASES \
    case BCHP_IRQ0_ANT_IRQEN: \
    case BCHP_IRQ0_GPIO_IRQEN: \
    case BCHP_IRQ0_SPI_IRQEN: \
    case BCHP_IRQ0_BSC_IRQEN:

#define    BINT_P_IRQ0_IRQ_CASE \
    case BCHP_IRQ0_IRQEN:

/* VSB AP cases */
#define BINT_P_AP_32_STATUS (0x0)
#define BINT_P_AP_32_ENABLE (0x10)

#define BINT_P_AP_32_CASES \
    case BCHP_AP_32_H_STAT1: \
    case BCHP_AP_32_H_STAT2: \
    case BCHP_AP_32_H_STAT4:

static void     BINT_P_35230_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void     BINT_P_35230_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void     BINT_P_35230_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void     BINT_P_35230_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_35230_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_35230_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

static const BINT_P_IntMap bint_35230[] =
{
    /* W0_STATUS */
#if (BCHP_CHIP==35230) || (BCHP_CHIP==35125)
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DVO_CPU_INTR_SHIFT,            BCHP_DVPO_INTR2_0_CPU_STATUS,         0,                   "DVO"         },
#endif
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_MEMSYS_CPU_INTR_SHIFT,         BCHP_MEMC_L2_0_R5F_STATUS,            0,                   "MEMC_0"      },
#if 0
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_TVM_CPU_INTR_SHIFT,            BCHP_TVM_INT_STATUS,                  0,                   "TVM"         },
#endif
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT,            BCHP_VIDEO_ENC_INTR2_CPU_STATUS,      0,                   "VEC"         },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_ADEC_CPU_INTR_SHIFT,           BCHP_ADEC_CORE_ADEC_INT_STATUS,       0,                   "ADEC"        },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DFE_CPU_INTR_SHIFT,            BCHP_DFE_MISCDEC_MISC_DFE_INT_STATUS, 0,                   "DFE"         },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVD0_CPU_INTR_SHIFT,           BCHP_AVD_INTR2_0_CPU_STATUS,          0,                   "AVD_0"       },
    /* BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_Reserved */
#if 0
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GENET_0_A_CPU_INTR_SHIFT,      BCHP_GENET_INTRL2_0_CPU_STATUS,       0,                   "GENET_A"     },
#endif
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_RX_0_CPU_INTR_SHIFT,      BCHP_HDMI_RX_INTR2_0_CPU_STATUS,      0,                   "HDMI"        },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DVP_HR_CPU_INTR_SHIFT,         BCHP_DVP_HR_INTR2_CPU_STATUS,         0,                   "DVP_HR"      },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_RAAGA_CPU_INTR_SHIFT,          BCHP_RAAGA_DSP_INTH_HOST_STATUS,      0,                   "RAAGA"       },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AIO_CPU_INTR_SHIFT,            BCHP_AIO_INTH_R5F_STATUS,             0,                   "AIO"         },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVN_CPU_INTR_SHIFT,            BCHP_BVNB_INTR2_CPU_STATUS,           0,                   "BVNB"        },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVN_CPU_INTR_5_SHIFT,          BCHP_BVNF_INTR2_5_R5F_STATUS,         0,                   "BVNF5"       },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVN_CPU_INTR_1_SHIFT,          BCHP_BVNF_INTR2_1_R5F_STATUS,         0,                   "BVNF1"       },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVN_CPU_INTR_0_SHIFT,          BCHP_BVNF_INTR2_0_R5F_STATUS,         0,                   "BVNF0"       },
#if (BCHP_CHIP==35230)
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_V3D_CPU_INTR_SHIFT,            BCHP_V3D_CTL_INTCTL,                  BINT_DONT_PROCESS_L2,"V3D"         },
#endif
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT,            BCHP_GFX_L2_CPU_STATUS,               0,                   "GFX"         },
#if ( BCHP_CHIP == 35230 && BCHP_VER >= BCHP_VER_C0 ) || (BCHP_CHIP == 35233)
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VDEC_REST_VDEC_INTERRUPT_CPU_INTR_SHIFT, BCHP_VDEC_REST_VDEC_INTERRUPT_STATUS, 0,         "VDEC"        },
#else
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVFE_VDEC_CPU_INTR_SHIFT,      BCHP_VDEC_REST_VDEC_INTERRUPT_STATUS, 0,                   "VDEC"        },
#endif
#if (BCHP_CHIP==35230)
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVFE_VIDBLK_CH2_CPU_INTR_SHIFT,BCHP_VIP_CAP1_L2_INT_CAP1_VIDBLK_CPU_INT_STATUS,0,         "VIDBLK_CH2"  },
#endif
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVFE_VIDBLK_CH1_CPU_INTR_SHIFT,BCHP_VIP_CAP0_L2_INT_CAP0_VIDBLK_CPU_INT_STATUS,0,         "VIDBLK_CH1"  },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVFE_VINDECO_CPU_INTR_SHIFT,   BCHP_VINDECO_INTR_CTRL2_CPU_STATUS,   0,                   "VINDECO"     },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MDMA_CPU_INTR_SHIFT,       BCHP_XPT_BUS_IF_INTR_STATUS2_REG,     0,                   "XPT_MDMA"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_STAT_CPU_INTR_SHIFT,   BCHP_XPT_MSG_DAT_ERR_INTR,            0,                   "XPT_MSG_ERR" },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_FE_CPU_INTR_SHIFT,         BCHP_XPT_FE_INTR_STATUS_REG,          0,                   "XPT_FE"      },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_PCR_CPU_INTR_SHIFT,        BCHP_XPT_FE_INTR_STATUS_REG,          0,                   "XPT_MDMA"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_PCR_CPU_INTR_SHIFT,        BCHP_XPT_DPCR0_INTR_STATUS_REG,       0,                   "XPT_DPCR0"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_PCR_CPU_INTR_SHIFT,        BCHP_XPT_DPCR1_INTR_STATUS_REG,       0,                   "XPT_DPCR1"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_PCR_CPU_INTR_SHIFT,        BCHP_XPT_DPCR2_INTR_STATUS_REG,       0,                   "XPT_DPCR2"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_PCR_CPU_INTR_SHIFT,        BCHP_XPT_DPCR3_INTR_STATUS_REG,       0,                   "XPT_DPCR3"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX0,                0,                    "RAV_CX0"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX1,                0,                    "RAV_CX1"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX2,                0,                    "RAV_CX2"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX3,                0,                    "RAV_CX3"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX4,                0,                    "RAV_CX4"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX5,                0,                    "RAV_CX5"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX6,                0,                    "RAV_CX6"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX7,                0,                    "RAV_CX7"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX8,                0,                    "RAV_CX8"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX9,                0,                    "RAV_CX9"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX10,               0,                    "RAV_CX10"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX11,               0,                    "RAV_CX11"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX12,               0,                    "RAV_CX12"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX13,               0,                    "RAV_CX13"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX14,               0,                    "RAV_CX14"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX15,               0,                    "RAV_CX15"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX16,               0,                    "RAV_CX16"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX17,               0,                    "RAV_CX17"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX18,               0,                    "RAV_CX18"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX19,               0,                    "RAV_CX19"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX20,               0,                    "RAV_CX20"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX21,               0,                    "RAV_CX21"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX22,               0,                    "RAV_CX22"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,        BCHP_XPT_RAVE_INT_CX23,               0,                    "RAV_CX23"   },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31,  BINT_DONT_PROCESS_L2, "XPT_MSG_0"  },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63,  BINT_DONT_PROCESS_L2, "XPT_MSG_1"  },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95,  BINT_DONT_PROCESS_L2, "XPT_MSG_2"  },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127, BINT_DONT_PROCESS_L2, "XPT_MSG_3"  },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,       BCHP_XPT_MSG_BUF_OVFL_INTR_00_31,     BINT_DONT_PROCESS_L2, "XPT_OVFL_0" },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,       BCHP_XPT_MSG_BUF_OVFL_INTR_32_63,     BINT_DONT_PROCESS_L2, "XPT_OVFL_1" },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,       BCHP_XPT_MSG_BUF_OVFL_INTR_64_95,     BINT_DONT_PROCESS_L2, "XPT_OVFL_2" },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,       BCHP_XPT_MSG_BUF_OVFL_INTR_96_127,    BINT_DONT_PROCESS_L2, "XPT_OVFL_3" },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS_REG,      0,                    "XPT"        },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS2_REG,     0,                    "XPT2"       },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS4_REG,     0,                    "XPT4"       },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_PB0_INTR,                    0,                    "XPT_PB0"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_PB1_INTR,                    0,                    "XPT_PB1"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_PB2_INTR,                    0,                    "XPT_PB2"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_PB3_INTR,                    0,                    "XPT_PB3"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_PB4_INTR,                    0,                    "XPT_PB4"    },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS5_REG,     0,                    "XPT5"       },
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT,            BCHP_HIF_INTR2_CPU_STATUS,            0xFFFFF000,           "HIF"        },
 
    /* W1_STATUS */
    /* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_Reserved */
    /* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_Reserved */
    /* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_Reserved */
    /* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_Reserved */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_AVS_CTRL_CPU_INTR_SHIFT+32,    BCHP_AVS_CTRL_STATUS,                 0,                    "AVS"        },
#if 0
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_OHCI_1_CPU_INTR_SHIFT+32,  BCHP_USB_OHCI_HcInterruptEnable,      0,                    "USB_OHCI_1" },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_EHCI_1_CPU_INTR_SHIFT+32,  BCHP_USB_EHCI_USBINTR,                0,                    "USB_EHCI_1" },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_OHCI_0_CPU_INTR_SHIFT+32,  BCHP_USB_OHCI_HcInterruptEnable,      0,                    "USB_OHCI_0" },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_EHCI_0_CPU_INTR_SHIFT+32,  BCHP_USB_EHCI_USBINTR,                0,                    "USB_EHCI_0" },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB_CPU_INTR_SHIFT+32,         BCHP_USB_INTR2_CPU_STATUS,            0,                    "USB"        },
#endif
#if (BCHP_CHIP==35230) && (BCHP_VER==BCHP_VER_C0)
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_FRC_CPU_INTR_SHIFT+32,         BCHP_FRC_CTLENG_INTR2_OUT_CPU_STATUS, 0,                    "FRC"        },
#endif
#if 0
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_TVM2MAIN_COMM_CPU_INTR_SHIFT+32,BCHP_PCU_PAR_INT_STAT,                0,                    "TVM2MAIN"   },
#endif
    /* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_Reserved */
    /* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_Reserved */
    /* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_Reserved */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RAAGA_FW_CPU_INTR_SHIFT+32,    BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS,   0,                    "RAAGA_FW"   },

#if 0
    /* TODO - NMI L2 register needs to be checked to see if it is correct */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_NMI_INTR_CPU_INTR_SHIFT+32,    BCHP_SUN_TOP_CTRL_NMI_CTRL_nmi_pad_monitor_MASK,0,          "NMI"        },

    /* TODO - MCIF L2 register not currently in RDB. Needs to be rechecked after RDB is updated */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_MC_CPU_INTR_SHIFT+32,          /*BCHP_MCIF_INTR2_CPU_STATUS*/0,      0,                    "MCIF"       },
#endif

    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_SC_CPU_INTR_SHIFT+32,      BCHP_SCIRQ0_SCIRQEN,                  0,                    "UPG_SC"     },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_TMR_CPU_INTR_SHIFT+32,     BCHP_TIMER_TIMER_IS,                  0,                    "UPG_TMR"    },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_CPU_INTR_SHIFT+32,         BCHP_IRQ0_IRQEN,                      0,                    "GPIO"       },
#if (BCHP_CHIP==35230) || (BCHP_CHIP==35125)
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SUN_CPU_INTR_SHIFT+32,         BCHP_SUN_L2_CPU_STATUS,               0,                    "SUN"        },
#endif
#if 0
    /* TODO - Fields below need to be updated when registers are part of RDB */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_BSC_C_CPU_INTR_SHIFT+32,   /*Missing status register in RDB*/0,  0,                    "PCU_BSC_C"  },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_BSC_B_CPU_INTR_SHIFT+32,   /*Missing status register in RDB*/0,  0,                    "PCU_BSC_B"  },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_UART3_CPU_INTR_SHIFT+32,   BCHP_PCU_UART3_IIR,                   0,                    "PCU_UART3"  },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_UART2_CPU_INTR_SHIFT+32,   BCHP_PCU_UART2_IIR,                   0,                    "PCU_UART2"  },
#endif
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_UIRT1_CPU_INTR_SHIFT+32,   BCHP_PCU_UIRT1_INTR_STATUS,           0,                    "PCU_UIRT1"  },

    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_SPI_CPU_INTR_SHIFT+32,     BCHP_PCU_SPI_CTRL,                    0,                    "PCU_SPI"    },
#if 1
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_DMA_CPU_INTR_SHIFT+32,     BCHP_PCU_DMA_INTR_STATUS,    0x00000002,                    "PCU_DMA"    },
#else
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_DMA_CPU_INTR_SHIFT+32,     BCHP_PCU_DMA_INTR_STATUS,             0,                    "PCU_DMA"    },
#endif

#if 0
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCU_OND_CPU_INTR_SHIFT+32,     BCHP_PCU_ONENAND_INT_STATUS,          0,                    "PCU_OND"    },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_IPI1_CPU_INTR_SHIFT+32,        BCHP_IPI1_INTR2_CPU_STATUS,           0,                    "IPI1"       },
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_IPI0_CPU_INTR_SHIFT+32,        BCHP_IPI0_INTR2_CPU_STATUS,           0,                    "IPI0"       },
    /* W2_STATUS */
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_GENET_0_B_CPU_INTR_SHIFT+64,   BCHP_GENET_INTRL2_1_CPU_STATUS,       0,                    "GENET_A"    },
#endif

    { -1, 0, 0, "Invalid" }
};

static const BINT_Settings bint_35230Settings =
{
    BINT_P_35230_SetInt,
    BINT_P_35230_ClearInt,
    BINT_P_35230_SetMask,
    BINT_P_35230_ClearMask,
    BINT_P_35230_ReadMask,
    BINT_P_35230_ReadStatus,
    bint_35230,
    "35230"
};

static void BINT_P_35230_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_STD_SET, ( 1ul<<shift ) );
            break;
        default:
            /* Only standard L2 interrupts support setting of interrupts */
            break;
    }
}

static void BINT_P_35230_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("ClearInt %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_STD_CLEAR, ( 1ul<<shift ) );
            break;
        BINT_P_XPT_STATUS_CASES
            /* Write 0 to clear the int bit. Writing 1's are ingored. */
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_STATUS, ~( 1ul<<shift ) );
            break;
        BINT_P_XPT_RAVE_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_RAVE_STATUS, ( 1ul<<shift ) );
            break;
        BINT_P_XPT_BUF_CASES
            /* Write 0 to clear the int bit. Writing 1's are ingored. */
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_STATUS, ~( 1ul<<shift ) );
            break;
        BINT_P_TIMER_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_STATUS, ( 1ul<<shift ) );
            break;
        BINT_P_UPGSC_CASES
        BINT_P_IRQ0_IRQ_CASE
            /* Has to cleared at the source */
            break;
        BINT_P_XPT_MSG_ERR_CASES
            /* Write 0 to clear the int bit. Writing 1's are ingored. */
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS, ~( 1ul << shift ) );
            break;
        BINT_P_DFE_ADEC_L2_CASES
            /* Write 1 to clear the int bit.  Writing 0's are ignored. */
            BREG_Write32( regHandle, baseAddr + BINT_P_DFE_ADEC_CLEAR, (1ul << shift) );
            break;
		
        default:
            /* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
            break;
    }
}

static void BINT_P_35230_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable = 0;
    uint32_t RaveEnReg = 0;

    BDBG_MSG(("SetMask %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_STD_MASK_SET, ( 1ul<<shift ) );
            break;
        BINT_P_XPT_STATUS_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_ENABLE );
            intEnable &= ~( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_ENABLE, intEnable );
            break;
        BINT_P_XPT_RAVE_CASES
            RaveEnReg  = GetRaveIntEnableOffset( baseAddr );
            intEnable  = BREG_Read32( regHandle, RaveEnReg );
            intEnable &= ~( 1ul<<shift );
            BREG_Write32( regHandle, RaveEnReg, intEnable );
            break;
        BINT_P_XPT_BUF_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE );
            intEnable &= ~( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE, intEnable );
            break;
        BINT_P_TIMER_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_ENABLE );
            intEnable &= ~( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_ENABLE, intEnable);
            break;
        BINT_P_IRQ0_IRQ_CASE
            intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE );
            intEnable &= ~(1ul<<shift);
            BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable );
            break;
        BINT_P_UPGSC_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
            intEnable &= ~( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE, intEnable );
            break;
        BINT_P_XPT_MSG_ERR_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
            intEnable &= ~( 1ul << shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE, intEnable);
            break;
        BINT_P_DFE_ADEC_L2_CASES
            /* AP uses enable logic - 0 = masked, 1 = not masked */
            intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_DFE_ADEC_ENABLE);
            intEnable &= ~( 1ul << shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_DFE_ADEC_ENABLE, intEnable );
            break;
        default:
            /* Unhandled interrupt base address */
            BDBG_ASSERT( false );
            break;
    }
}

static void BINT_P_35230_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable = 0;
    uint32_t RaveEnReg = 0;

    BDBG_MSG(("ClearMask %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_STD_MASK_CLEAR, ( 1ul<<shift ) );
            break;
        BINT_P_XPT_STATUS_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_ENABLE );
            intEnable |= ( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_ENABLE, intEnable );
            break;
        BINT_P_XPT_RAVE_CASES
            RaveEnReg  = GetRaveIntEnableOffset( baseAddr );
            intEnable  = BREG_Read32( regHandle, RaveEnReg );
            intEnable |= ( 1ul<<shift );
            BREG_Write32( regHandle, RaveEnReg, intEnable );
            break;
        BINT_P_XPT_BUF_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE );
            intEnable |= ( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE, intEnable );
            break;
        BINT_P_UPGSC_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
            intEnable |= ( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE, intEnable );
            break;
        BINT_P_IRQ0_IRQ_CASE
            intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
            intEnable |= 1ul<<shift;
            BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable );
            break;

        BINT_P_TIMER_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_ENABLE );
            intEnable |= ( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_ENABLE, intEnable );
            break;
        BINT_P_XPT_MSG_ERR_CASES
            intEnable  = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
            intEnable |= ( 1ul<<shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE, intEnable );
            break;
        BINT_P_DFE_ADEC_L2_CASES
            /* AP uses enable logic - 0 = masked, 1 = not masked */
            intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_DFE_ADEC_ENABLE );
            intEnable |= ( 1ul << shift );
            BREG_Write32( regHandle, baseAddr + BINT_P_DFE_ADEC_ENABLE, intEnable );
            break;
        default:
            /* Unhandled interrupt base address */
            BDBG_ASSERT( false );
            break;
    }
}

static uint32_t BINT_P_35230_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
{
    uint32_t RaveEnReg = 0;

    BDBG_MSG(("ReadMask %#x", baseAddr));
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            return BREG_Read32(regHandle, baseAddr + BINT_P_STD_MASK_STATUS);
        BINT_P_XPT_STATUS_CASES
            return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_ENABLE );
        BINT_P_XPT_RAVE_CASES
            RaveEnReg = GetRaveIntEnableOffset( baseAddr );
            return ~BREG_Read32( regHandle, RaveEnReg );
        BINT_P_XPT_BUF_CASES
            return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE );
        BINT_P_TIMER_CASES
            return ~BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_ENABLE );
        BINT_P_IRQ0_IRQ_CASE
            return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE );
        BINT_P_UPGSC_CASES
            return ~BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
        BINT_P_XPT_MSG_ERR_CASES
            return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
        BINT_P_DFE_ADEC_L2_CASES
            /* AP uses enable logic - 0 = masked, 1 = not masked */
            return ~BREG_Read32( regHandle, baseAddr + BINT_P_DFE_ADEC_ENABLE);
            break;

        default:
            /* Unhandled interrupt base address */
            BDBG_ASSERT( false );
            return 0;
    }
}

static uint32_t BINT_P_35230_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
    BDBG_MSG(("ReadStatus %#x", baseAddr));
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            return BREG_Read32( regHandle, baseAddr + BINT_P_STD_STATUS );
        BINT_P_XPT_STATUS_CASES
            return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_STATUS );
        BINT_P_XPT_RAVE_CASES
            return BREG_Read32( regHandle, baseAddr + BINT_P_RAVE_STATUS );
        BINT_P_XPT_BUF_CASES
            return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_STATUS );
        BINT_P_TIMER_CASES
            return BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_STATUS );
        BINT_P_IRQ0_IRQ_CASE
            return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS );
        BINT_P_UPGSC_CASES
            return BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_STATUS );
        BINT_P_XPT_MSG_ERR_CASES
            return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS );
        BINT_P_DFE_ADEC_L2_CASES
            /* AP uses enable logic - 0 = masked, 1 = not masked */
            return BREG_Read32( regHandle, baseAddr + BINT_P_DFE_ADEC_STATUS );
        default:
            /* Unhandled interrupt base address */
            BDBG_ASSERT( false );
            return 0;
    }
}

static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr )
{
    uint32_t EnableAddr = 0;

    switch( BaseAddr )
    {
        case BCHP_XPT_RAVE_INT_CX0:  EnableAddr =  BCHP_XPT_RAVE_CX0_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX1:  EnableAddr =  BCHP_XPT_RAVE_CX1_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX2:  EnableAddr =  BCHP_XPT_RAVE_CX2_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX3:  EnableAddr =  BCHP_XPT_RAVE_CX3_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX4:  EnableAddr =  BCHP_XPT_RAVE_CX4_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX5:  EnableAddr =  BCHP_XPT_RAVE_CX5_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX6:  EnableAddr =  BCHP_XPT_RAVE_CX6_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX7:  EnableAddr =  BCHP_XPT_RAVE_CX7_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX8:  EnableAddr =  BCHP_XPT_RAVE_CX8_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX9:  EnableAddr =  BCHP_XPT_RAVE_CX9_AV_INTERRUPT_ENABLES;  break;
        case BCHP_XPT_RAVE_INT_CX10: EnableAddr =  BCHP_XPT_RAVE_CX10_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX11: EnableAddr =  BCHP_XPT_RAVE_CX11_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX12: EnableAddr =  BCHP_XPT_RAVE_CX12_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX13: EnableAddr =  BCHP_XPT_RAVE_CX13_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX14: EnableAddr =  BCHP_XPT_RAVE_CX14_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX15: EnableAddr =  BCHP_XPT_RAVE_CX15_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX16: EnableAddr =  BCHP_XPT_RAVE_CX16_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX17: EnableAddr =  BCHP_XPT_RAVE_CX17_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX18: EnableAddr =  BCHP_XPT_RAVE_CX18_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX19: EnableAddr =  BCHP_XPT_RAVE_CX19_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX20: EnableAddr =  BCHP_XPT_RAVE_CX20_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX21: EnableAddr =  BCHP_XPT_RAVE_CX21_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX22: EnableAddr =  BCHP_XPT_RAVE_CX22_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX23: EnableAddr =  BCHP_XPT_RAVE_CX23_AV_INTERRUPT_ENABLES; break;

        default:
            /* Unhandled interrupt base address */
            BDBG_ASSERT( false );
            break;
    }

    return EnableAddr;
}


#if (BCHP_CHIP==35230)
const BINT_Settings *BINT_35230_GetSettings( void )
#elif (BCHP_CHIP==35125)
const BINT_Settings *BINT_35125_GetSettings( void )
#elif (BCHP_CHIP==35233)
const BINT_Settings *BINT_35233_GetSettings( void )
#endif
{
    return &bint_35230Settings;
}

/* End of file */