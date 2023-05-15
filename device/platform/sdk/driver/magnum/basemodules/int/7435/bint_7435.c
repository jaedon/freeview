/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7435.c $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 10/12/12 3:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7435/bint_7435.c $
 * 
 * Hydra_Software_Devel/17   10/12/12 3:48p mward
 * SW7435-420: Fix suffix naming convention.
 * 
 * Hydra_Software_Devel/16   10/5/12 11:13a mphillip
 * SW7425-3963: Merge interrupt changes to main
 * 
 * Hydra_Software_Devel/SW7425-3963/1   9/28/12 8:11p mphillip
 * SW7425-3963: Update interrupt handling with optimizations and support
 * for 'weak' interrupt masking
 * 
 * Hydra_Software_Devel/15   9/17/12 1:16p hauxwell
 * SW7435-352 : Nexus interrupt map needs changing to remove programming
 * of V3D L2
 * 
 * Hydra_Software_Devel/14   9/11/12 2:30p albertl
 * SW7435-12: Added additional MEMC interrupts to the interrupt map.
 * 
 * Hydra_Software_Devel/13   6/4/12 4:23p erickson
 * SW7435-27: use BINT for core1 interrupts
 * 
 * Hydra_Software_Devel/12   5/8/12 3:38p erickson
 * SW7435-27: in webcpu mode, core0 doesn't get M2MC1 L1
 * 
 * Hydra_Software_Devel/11   4/20/12 9:45p agin
 * SWNOOS-518:  Support 97435.
 * 
 * Hydra_Software_Devel/10   3/12/12 5:36p mward
 * SW7435-3:  RDB update removed unnecessary AIO registers.
 * 
 * Hydra_Software_Devel/9   3/5/12 12:38p mward
 * SW7435-26:  Corrected/enabled support for SID.
 * 
 * Hydra_Software_Devel/8   3/1/12 3:07p nilesh
 * SW7435-3: Fixed VICE_1 interrupt
 * 
 * Hydra_Software_Devel/7   1/4/12 11:31a mward
 * SW7435-3:  Update register naming for M2MC and VICE2.
 * 
 * Hydra_Software_Devel/6   12/19/11 6:26p mward
 * SW7435-29:  Remove outdated UPG Smartcard interrupt implementation.
 * 
 * Hydra_Software_Devel/5   12/15/11 5:50p mward
 * SW7435-28:  Allow configuring both UART A and UART B for handling by OS
 * other than Linux.
 * 
 * Hydra_Software_Devel/4   11/9/11 5:55p mward
 * SW7435-7:  M2MC L2 register name changes.
 * 
 * Hydra_Software_Devel/3   10/28/11 1:55p mward
 * SW7435-7:  Fix BVNF_INTR2_6 names.
 * 
 * Hydra_Software_Devel/2   10/27/11 5:57p mward
 * SW7435-7: Update.
 * 
 * Hydra_Software_Devel/1   10/21/11 4:53p mward
 * SW7435-7: Add 7435.  Initial version.
 * 
 * Hydra_Software_Devel/8   9/13/11 12:12a hongtaoz
 * SW7425-1248: fixed V3D L1 bit mask for B0;
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bint_7435.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"

/* Standard L2 stuff */
#include "bchp_aio_inth.h"
#include "bchp_avd_intr2_1.h"
#include "bchp_bsp_control_intr2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_bvnf_intr2_6.h"
#include "bchp_bvnf_intr2_7.h"
#include "bchp_bvnm_intr2_0.h"
#include "bchp_bvnm_intr2_1.h"
#include "bchp_clkgen_intr2.h"
#include "bchp_dvp_hr_intr2.h"
#include "bchp_m2mc_l2.h"
#include "bchp_m2mc1_l2.h"
#include "bchp_hdmi_rx_intr2_0.h"
#include "bchp_hdmi_tx_intr2.h"
#include "bchp_hif_spi_intr2.h"
#include "bchp_hif_intr2.h"
#include "bchp_memc_l2_0.h"
#include "bchp_memc_l2_1.h"
#include "bchp_memc_l2_1_0.h"
#include "bchp_memc_l2_1_1.h"
#include "bchp_raaga_dsp_inth.h"
#include "bchp_raaga_dsp_inth_1.h"
#include "bchp_raaga_dsp_fw_inth.h"
#include "bchp_raaga_dsp_fw_inth_1.h"
#include "bchp_sm_l2.h"
#include "bchp_svd_intr2_0.h"
#include "bchp_sun_l2.h"
#include "bchp_aon_l2.h"
#include "bchp_aon_pm_l2.h"
#include "bchp_uhfr_intr2.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_vice2_l2_0.h"
#include "bchp_vice2_l2_1.h"
#include "bchp_mcif_intr2.h"
#include "bchp_upg_aux_intr2.h"
#include "bchp_sid_l2.h"
#include "bchp_upg_aux_aon_intr2.h"

/* XPT */
#include "bchp_xpt_fe.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bchp_xpt_dpcr2.h"
#include "bchp_xpt_dpcr3.h"
#include "bchp_xpt_dpcr4.h"
#include "bchp_xpt_dpcr5.h"
#include "bchp_xpt_dpcr6.h"
#include "bchp_xpt_dpcr7.h"
#include "bchp_xpt_dpcr8.h"
#include "bchp_xpt_dpcr9.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"
#include "bchp_xpt_pb2.h"
#include "bchp_xpt_pb3.h"
#include "bchp_xpt_pb4.h"
#include "bchp_xpt_pb5.h"
#include "bchp_xpt_pb6.h"
#include "bchp_xpt_pb7.h"
#include "bchp_xpt_pb8.h"
#include "bchp_xpt_pb9.h"
#include "bchp_xpt_pb10.h"
#include "bchp_xpt_pb11.h"
#include "bchp_xpt_pb12.h"
#include "bchp_xpt_pb13.h"
#include "bchp_xpt_pb14.h"
#include "bchp_xpt_pb15.h"
#include "bchp_xpt_pb16.h"
#include "bchp_xpt_pb17.h"
#include "bchp_xpt_pb18.h"
#include "bchp_xpt_pb19.h"
#include "bchp_xpt_pb20.h"
#include "bchp_xpt_pb21.h"
#include "bchp_xpt_rave.h"
#include "bchp_xpt_msg.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_xpt_wakeup.h"
#include "bchp_xpt_full_pid_parser.h"

/* UARTs, keypad, I2C */
#include "bchp_irq0.h"
#include "bchp_irq0_aon.h"

/* Smartcard interrupts. */
#include "bchp_scirq0.h"

/* Timer */
#include "bchp_timer.h"

BDBG_MODULE(interruptinterface_7435);

#define BINT_P_STD_STATUS       0x00
#define BINT_P_STD_SET          0x04
#define BINT_P_STD_CLEAR        0x08
#define BINT_P_STD_MASK_STATUS  0x0C
#define BINT_P_STD_MASK_SET     0x10
#define BINT_P_STD_MASK_CLEAR   0x14


#define BINT_P_IRQ0_CASES \
    case BCHP_IRQ0_IRQEN:

#define BINT_P_IRQ0_ENABLE      0
#define BINT_P_IRQ0_STATUS      4


#define BINT_P_IRQ0_AON_CASES \
    case BCHP_IRQ0_AON_IRQEN:

#define BINT_P_IRQ0_AON_ENABLE      0
#define BINT_P_IRQ0_AON_STATUS      4


#define BINT_P_XPT_STATUS           0x00
#define BINT_P_XPT_ENABLE           0x04

#define BINT_P_XPT_STATUS_CASES \
    case BCHP_XPT_BUS_IF_INTR_STATUS_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS2_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS3_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS4_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS5_REG: \
    case BCHP_XPT_PB0_INTR: \
    case BCHP_XPT_PB1_INTR: \
    case BCHP_XPT_PB2_INTR: \
    case BCHP_XPT_PB3_INTR: \
    case BCHP_XPT_PB4_INTR: \
    case BCHP_XPT_PB5_INTR: \
    case BCHP_XPT_PB6_INTR: \
    case BCHP_XPT_PB7_INTR: \
    case BCHP_XPT_PB8_INTR: \
    case BCHP_XPT_PB9_INTR: \
    case BCHP_XPT_PB10_INTR: \
    case BCHP_XPT_PB11_INTR: \
    case BCHP_XPT_PB12_INTR: \
    case BCHP_XPT_PB13_INTR: \
    case BCHP_XPT_PB14_INTR: \
    case BCHP_XPT_PB15_INTR: \
    case BCHP_XPT_PB16_INTR: \
    case BCHP_XPT_PB17_INTR: \
    case BCHP_XPT_PB18_INTR: \
    case BCHP_XPT_PB19_INTR: \
    case BCHP_XPT_PB20_INTR: \
    case BCHP_XPT_PB21_INTR: \
    case BCHP_XPT_FE_INTR_STATUS0_REG: \
    case BCHP_XPT_FE_INTR_STATUS1_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_PCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_PCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_SCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_SCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_PSG_PROTOCOL_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_PSG_PROTOCOL_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_TRANSPORT_ERROR_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_TRANSPORT_ERROR_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR0_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR1_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR2_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR3_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR4_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR5_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR6_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR7_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR8_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR9_INTR_STATUS_REG: \
    case BCHP_XPT_WAKEUP_INTR_STATUS_REG:

#define BINT_P_PCROFFSET_CASES \
    case BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS: \
    case BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS: \
    case BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS: \
    case BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS:

#define BINT_P_PCROFFSET_STATUS     0x00
#define BINT_P_PCROFFSET_ENABLE     0x04

/* There is no constant address mapping from RAVE status to RAVE enable registers. */
#define BINT_P_RAVE_STATUS          0x00

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
    case BCHP_XPT_RAVE_INT_CX23: \
    case BCHP_XPT_RAVE_INT_CX24: \
    case BCHP_XPT_RAVE_INT_CX25: \
    case BCHP_XPT_RAVE_INT_CX26: \
    case BCHP_XPT_RAVE_INT_CX27: \
    case BCHP_XPT_RAVE_INT_CX28: \
    case BCHP_XPT_RAVE_INT_CX29: \
    case BCHP_XPT_RAVE_INT_CX30: \
    case BCHP_XPT_RAVE_INT_CX31: \
    case BCHP_XPT_RAVE_INT_CX32: \
    case BCHP_XPT_RAVE_INT_CX33: \
    case BCHP_XPT_RAVE_INT_CX34: \
    case BCHP_XPT_RAVE_INT_CX35: \
    case BCHP_XPT_RAVE_INT_CX36: \
    case BCHP_XPT_RAVE_INT_CX37: \
    case BCHP_XPT_RAVE_INT_CX38: \
    case BCHP_XPT_RAVE_INT_CX39: \
    case BCHP_XPT_RAVE_INT_CX40: \
    case BCHP_XPT_RAVE_INT_CX41: \
    case BCHP_XPT_RAVE_INT_CX42: \
    case BCHP_XPT_RAVE_INT_CX43: \
    case BCHP_XPT_RAVE_INT_CX44: \
    case BCHP_XPT_RAVE_INT_CX45: \
    case BCHP_XPT_RAVE_INT_CX46: \
    case BCHP_XPT_RAVE_INT_CX47: \

#define BINT_P_XPT_BUF_STATUS       0x00
#define BINT_P_XPT_BUF_ENABLE       0x10

#define BINT_P_XPT_BUF_CASES \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_00_31: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_32_63: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_64_95: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_96_127: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127:

#define BINT_P_XPT_MSG_ERR_STATUS   ( 0x00 )
#define BINT_P_XPT_MSG_ERR_ENABLE   ( 0x04 )
#define BINT_P_XPT_MSG_ERR_CASES \
    case BCHP_XPT_MSG_DAT_ERR_INTR:

/* BINT_P_UPGSC_ENABLE was defined as -4 for BCHP_SCIRQ0_SCIRQSTAT.
 * Since we are using BCHP_SCIRQ0_SCIRQEN, it is not needed but
 * to minimize the change, it is kept and set to 0
 */
#define BINT_P_UPGSC_ENABLE (0)

#define BINT_P_UPGSC_CASES \
    case BCHP_SCIRQ0_SCIRQEN:

#define BINT_P_TIMER_STATUS     0x00
#define BINT_P_TIMER_MASK       0x04

#define BINT_P_TIMER_CASES \
    case BCHP_TIMER_TIMER_IS:

#define BINT_P_STAT_TIMER_TICKS_PER_USEC 27

static void BINT_P_7435_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7435_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7435_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7435_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

#if NEXUS_WEBCPU_core1_server
static const BINT_P_IntMap bint_7435[] =
{
    BINT_MAP_STD(3, M2MC1, M2MC1_L2_CPU),
    { -1, 0, 0, NULL}
};
#else
static const BINT_P_IntMap bint_7435[] =
{
    BINT_MAP_STD(0, MEMC1, MEMC_L2_1_R5F),
    BINT_MAP_STD(0, MEMC1, MEMC_L2_1_1_R5F),
    BINT_MAP_STD(0, MEMC0, MEMC_L2_0_R5F),
    BINT_MAP_STD(0, MEMC0, MEMC_L2_1_0_R5F),

    BINT_MAP_STD(0, HDMI_TX, HDMI_TX_INTR2_CPU),
    BINT_MAP_STD(0, HDMI_RX_0, HDMI_RX_INTR2_0_CPU),

    BINT_MAP_STD(0, GFX, M2MC_L2_CPU),

    BINT_MAP_STD(0, DVP_HR, DVP_HR_INTR2_CPU),

    BINT_MAP_STD(0, CLKGEN, CLKGEN_INTR2_CPU),
    BINT_MAP_STD(0, BVNM_1, BVNM_INTR2_1_R5F),
    BINT_MAP_STD(0, BVNM_0, BVNM_INTR2_0_R5F),
    BINT_MAP_STD(0, BVNF_5, BVNF_INTR2_5_R5F),
    BINT_MAP_STD(0, BVNF_1, BVNF_INTR2_1_R5F),
    BINT_MAP_STD(0, BVNF_0, BVNF_INTR2_0_R5F),
    BINT_MAP_STD(0, BVNB_0, BVNB_INTR2_CPU),

    BINT_MAP_STD(0, BSP, BSP_CONTROL_INTR2_CPU),

    BINT_MAP_STD(0, AVD1_0, AVD_INTR2_1_CPU),

    BINT_MAP_STD(0, AIO, AIO_INTH_R5F),

    BINT_MAP(1, UPG_SC, "", SCIRQ0_SCIRQEN, REGULAR, ALL, 0),
    BINT_MAP(1, UPG_MAIN_AON, "" , IRQ0_AON_IRQEN, REGULAR, MASK, 0xFFFFFE1C),
    BINT_MAP(1, UPG_MAIN, "" , IRQ0_IRQEN, REGULAR, MASK, 0xFFFFFC00),
    BINT_MAP(1, UPG_BSC_AON, "" , IRQ0_AON_IRQEN, REGULAR, MASK, 0xE7FFFFFF),
    BINT_MAP(1, UPG_AUX_AON, "", UPG_AUX_AON_INTR2_CPU_STATUS, STANDARD, ALL, 0),
    BINT_MAP(1, UPG_BSC, "" , IRQ0_IRQEN, REGULAR, MASK, 0xF8FFFFFF),
    /* Combined into a single entry due to BINT requirements...
    BINT_MAP(1, UPG_AUX, "_CTK", UPG_AUX_INTR2_CPU_STATUS, STANDARD, MASK, 0xFFFFFE00),
    BINT_MAP(1, UPG_AUX, "_TMON", UPG_AUX_INTR2_CPU_STATUS, STANDARD, MASK, 0xFFFF9FFF), */
    BINT_MAP(1, UPG_AUX, "_CTK_UPG_AUX_TMON", UPG_AUX_INTR2_CPU_STATUS, STANDARD, MASK, 0xFFFF9E00),

    BINT_MAP_STD(1, UHF, UHFR_INTR2_CPU),

    /* BINT_MAP_STD(1, SYS_PM, AON_PM_L2_CPU), */
    BINT_MAP_STD(1, SYS_AON, AON_L2_CPU),
    BINT_MAP_STD(1, SYS, SUN_L2_CPU),

    BINT_MAP_STD(1, SVD0_0, SVD_INTR2_0_CPU),

    BINT_MAP_STD(1, SOFT_MODEM, SM_L2_CPU),

    BINT_MAP_STD(1, SID0_0, SID_L2_CPU),

    BINT_MAP_STD(1, RAAGA1_FW, RAAGA_DSP_FW_INTH_1_HOST),
    BINT_MAP_STD(1, RAAGA1, RAAGA_DSP_INTH_1_HOST),
    BINT_MAP_STD(1, RAAGA_FW, RAAGA_DSP_FW_INTH_HOST),
    BINT_MAP_STD(1, RAAGA, RAAGA_DSP_INTH_HOST),

    BINT_MAP(2, XPT_STATUS, "" , XPT_BUS_IF_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "2", XPT_BUS_IF_INTR_STATUS2_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "3", XPT_BUS_IF_INTR_STATUS3_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "4", XPT_BUS_IF_INTR_STATUS4_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "5", XPT_BUS_IF_INTR_STATUS5_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB0", XPT_PB0_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB1", XPT_PB1_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB2", XPT_PB2_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB3", XPT_PB3_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB4", XPT_PB4_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB5", XPT_PB5_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB6", XPT_PB6_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB7", XPT_PB7_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB8", XPT_PB8_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB9", XPT_PB9_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB10", XPT_PB10_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB11", XPT_PB11_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB12", XPT_PB12_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB13", XPT_PB13_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB14", XPT_PB14_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB15", XPT_PB15_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB16", XPT_PB16_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB17", XPT_PB17_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB18", XPT_PB18_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB19", XPT_PB19_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB20", XPT_PB20_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_PB21", XPT_PB21_INTR, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_STATUS, "_WAKEUP", XPT_WAKEUP_INTR_STATUS_REG, REGULAR, ALL, 0),

    BINT_MAP(2, XPT_RAV, "_CX0", XPT_RAVE_INT_CX0, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX1", XPT_RAVE_INT_CX1, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX2", XPT_RAVE_INT_CX2, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX3", XPT_RAVE_INT_CX3, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX4", XPT_RAVE_INT_CX4, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX5", XPT_RAVE_INT_CX5, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX6", XPT_RAVE_INT_CX6, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX7", XPT_RAVE_INT_CX7, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX8", XPT_RAVE_INT_CX8, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX9", XPT_RAVE_INT_CX9, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX10", XPT_RAVE_INT_CX10, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX11", XPT_RAVE_INT_CX11, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX12", XPT_RAVE_INT_CX12, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX13", XPT_RAVE_INT_CX13, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX14", XPT_RAVE_INT_CX14, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX15", XPT_RAVE_INT_CX15, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX16", XPT_RAVE_INT_CX16, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX17", XPT_RAVE_INT_CX17, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX18", XPT_RAVE_INT_CX18, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX19", XPT_RAVE_INT_CX19, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX20", XPT_RAVE_INT_CX20, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX21", XPT_RAVE_INT_CX21, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX22", XPT_RAVE_INT_CX22, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX23", XPT_RAVE_INT_CX23, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX24", XPT_RAVE_INT_CX24, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX25", XPT_RAVE_INT_CX25, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX26", XPT_RAVE_INT_CX26, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX27", XPT_RAVE_INT_CX27, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX28", XPT_RAVE_INT_CX28, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX29", XPT_RAVE_INT_CX29, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX30", XPT_RAVE_INT_CX30, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX31", XPT_RAVE_INT_CX31, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX32", XPT_RAVE_INT_CX32, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX33", XPT_RAVE_INT_CX33, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX34", XPT_RAVE_INT_CX34, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX35", XPT_RAVE_INT_CX35, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX36", XPT_RAVE_INT_CX36, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX37", XPT_RAVE_INT_CX37, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX38", XPT_RAVE_INT_CX38, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX39", XPT_RAVE_INT_CX39, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX40", XPT_RAVE_INT_CX40, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX41", XPT_RAVE_INT_CX41, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX42", XPT_RAVE_INT_CX42, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX43", XPT_RAVE_INT_CX43, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX44", XPT_RAVE_INT_CX44, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX45", XPT_RAVE_INT_CX45, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX46", XPT_RAVE_INT_CX46, WEAK_MASK, ALL, 0),
    BINT_MAP(2, XPT_RAV, "_CX47", XPT_RAVE_INT_CX47, WEAK_MASK, ALL, 0),
    /* don't process it for now */
    BINT_MAP(2, XPT_RAV, "", XPT_RAVE_INT_MISC, REGULAR, NONE, 0),

    BINT_MAP(2, XPT_PCR, "0", XPT_DPCR0_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "1", XPT_DPCR1_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "2", XPT_DPCR2_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "3", XPT_DPCR3_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "4", XPT_DPCR4_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "5", XPT_DPCR5_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "6", XPT_DPCR6_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "7", XPT_DPCR7_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "8", XPT_DPCR8_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "9", XPT_DPCR9_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "OFF0", XPT_PCROFFSET_INTERRUPT0_STATUS, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "OFF1", XPT_PCROFFSET_INTERRUPT1_STATUS, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "OFF2", XPT_PCROFFSET_INTERRUPT2_STATUS, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "OFF3", XPT_PCROFFSET_INTERRUPT3_STATUS, REGULAR, ALL, 0),

    BINT_MAP(2, XPT_OVFL, "_0", XPT_MSG_BUF_OVFL_INTR_00_31,  REGULAR, NONE, 0),
    BINT_MAP(2, XPT_OVFL, "_1", XPT_MSG_BUF_OVFL_INTR_32_63,  REGULAR, NONE, 0),
    BINT_MAP(2, XPT_OVFL, "_2", XPT_MSG_BUF_OVFL_INTR_64_95,  REGULAR, NONE, 0),
    BINT_MAP(2, XPT_OVFL, "_3", XPT_MSG_BUF_OVFL_INTR_96_127,  REGULAR, NONE,0),

    BINT_MAP(2, XPT_MSG_STAT, "", XPT_MSG_DAT_ERR_INTR,       REGULAR, ALL, 0),

    BINT_MAP(2, XPT_MSG, "_0", XPT_MSG_BUF_DAT_RDY_INTR_00_31,       REGULAR, NONE, 0),
    BINT_MAP(2, XPT_MSG, "_1", XPT_MSG_BUF_DAT_RDY_INTR_32_63,       REGULAR, NONE, 0),
    BINT_MAP(2, XPT_MSG, "_2", XPT_MSG_BUF_DAT_RDY_INTR_64_95,       REGULAR, NONE, 0),
    BINT_MAP(2, XPT_MSG, "_3", XPT_MSG_BUF_DAT_RDY_INTR_96_127,      REGULAR, NONE, 0),

    BINT_MAP(2, XPT_FE, "_STATUS0", XPT_FE_INTR_STATUS0_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_STATUS1", XPT_FE_INTR_STATUS1_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_IBP_PCC", XPT_FULL_PID_PARSER_IBP_PCC_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_PBP_PCC", XPT_FULL_PID_PARSER_PBP_PCC_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_IBP_SCC", XPT_FULL_PID_PARSER_IBP_SCC_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_PBP_SCC", XPT_FULL_PID_PARSER_PBP_SCC_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_IBP_PSG", XPT_FULL_PID_PARSER_IBP_PSG_PROTOCOL_INTR_STATUS_REG,   REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_PBP_PSG", XPT_FULL_PID_PARSER_PBP_PSG_PROTOCOL_INTR_STATUS_REG, REGULAR, ALL, 0  ),
    BINT_MAP(2, XPT_FE, "_IBP_TRANSPORT_ERROR", XPT_FULL_PID_PARSER_IBP_TRANSPORT_ERROR_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_FE, "_PBP_TRANSPORT_ERROR", XPT_FULL_PID_PARSER_PBP_TRANSPORT_ERROR_INTR_STATUS_REG, REGULAR, ALL, 0),

#ifndef BINT_VICE2_DISABLED
    BINT_MAP_STD(2, VICE1_0, VICE2_L2_1_CPU),
    BINT_MAP_STD(2, VICE0_0, VICE2_L2_0_CPU),
#endif

    BINT_MAP_STD(2, VEC, VIDEO_ENC_INTR2_CPU),

    BINT_MAP(2, UPG_TMR, "" , TIMER_TIMER_IS, REGULAR, ALL, 0),
    BINT_MAP(2, UPG_SPI, "" , IRQ0_AON_IRQEN, REGULAR, MASK, 0xFFEFFFFF),

#if !NEXUS_WEBCPU
/* in webcpu mode, core0 doesn't get M2MC1 L1 */
    BINT_MAP_STD(3, M2MC1, M2MC1_L2_CPU),
#endif

    { -1, 0, 0, NULL}
};
#endif

static const BINT_Settings bint_7435Settings =
{
    NULL,
    BINT_P_7435_ClearInt,
    BINT_P_7435_SetMask,
    BINT_P_7435_ClearMask,
    NULL,
    BINT_P_7435_ReadStatus,
    bint_7435,
    "7435"
};

static void BINT_P_7435_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("ClearInt %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
        BINT_P_XPT_STATUS_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_STATUS, ~(1ul<<shift));
            break;
        BINT_P_XPT_RAVE_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_RAVE_STATUS, (1ul<<shift));
            break;
        BINT_P_XPT_BUF_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_STATUS, ~(1ul<<shift));
            break;
        BINT_P_TIMER_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_STATUS, 1ul<<shift);
            break;
        BINT_P_UPGSC_CASES
        BINT_P_IRQ0_CASES
        BINT_P_IRQ0_AON_CASES
            /* Has to cleared at the source */
            break;
        BINT_P_XPT_MSG_ERR_CASES
            /* Write 0 to clear the int bit. Writing 1's are ingored. */
            BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS, ~( 1ul << shift ) );
            break;
        BINT_P_PCROFFSET_CASES
            /* Write 0 to clear the int bit. Writing 1's are ingored. */
            BREG_Write32( regHandle, baseAddr + BINT_P_PCROFFSET_STATUS, ~( 1ul << shift ) );
            break;
        default:
            /* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
            break;
    }
}

static void BINT_P_7435_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable;

    uint32_t RaveEnReg = 0;

    BDBG_MSG(("SetMask %#x:%d", baseAddr, shift));

    switch( baseAddr )
    {
    BINT_P_XPT_STATUS_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_ENABLE);
        intEnable &= ~(1ul<<shift);
        BREG_Write32( regHandle, baseAddr + BINT_P_XPT_ENABLE, intEnable);
        break;
    BINT_P_XPT_RAVE_CASES
        RaveEnReg = GetRaveIntEnableOffset( baseAddr );
        intEnable = BREG_Read32( regHandle, RaveEnReg );
        intEnable &= ~(1ul<<shift);
        BREG_Write32( regHandle, RaveEnReg, intEnable);
        break;
    BINT_P_XPT_BUF_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE);
        intEnable &= ~(1ul<<shift);
        BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE, intEnable);
        break;
    BINT_P_TIMER_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK);
        intEnable &= ~(1ul<<shift);
        BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_MASK, intEnable);
        break;
    BINT_P_IRQ0_CASES
        /* we need to keep uart a/b interrupts enabled */
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
        intEnable &= ~(1ul<<shift);
#ifndef UART_A_SUPPORT
        intEnable |= BCHP_IRQ0_IRQSTAT_uairq_MASK;
#endif
#ifndef UART_B_SUPPORT
        intEnable |= BCHP_IRQ0_IRQSTAT_ubirq_MASK;
#endif
        BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable);
        break;
    BINT_P_IRQ0_AON_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_AON_ENABLE);
        intEnable &= ~(1ul<<shift);
        BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_AON_ENABLE, intEnable);
        break;
    BINT_P_UPGSC_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
        intEnable &= ~(1ul<<shift);
        BREG_Write32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE, intEnable );
        break;

    BINT_P_XPT_MSG_ERR_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
        intEnable &= ~( 1ul << shift );
        BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE, intEnable);
        break;

    BINT_P_PCROFFSET_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_PCROFFSET_ENABLE );
        intEnable &= ~( 1ul << shift );
        BREG_Write32( regHandle, baseAddr + BINT_P_PCROFFSET_ENABLE, intEnable);
        break;

    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        break;
    }
}

static void BINT_P_7435_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable;

    uint32_t RaveEnReg = 0;

    BDBG_MSG(("ClearMask %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
    BINT_P_XPT_STATUS_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_ENABLE);
        intEnable |= 1ul<<shift;
        BREG_Write32( regHandle, baseAddr + BINT_P_XPT_ENABLE, intEnable);
        break;
    BINT_P_XPT_RAVE_CASES
        RaveEnReg = GetRaveIntEnableOffset( baseAddr );
        intEnable = BREG_Read32( regHandle, RaveEnReg );
        intEnable |= (1ul<<shift);
        BREG_Write32( regHandle, RaveEnReg, intEnable);
        break;
    BINT_P_XPT_BUF_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE);
        intEnable |= 1ul<<shift;
        BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE, intEnable);
        break;
    BINT_P_IRQ0_CASES
        /* we need to keep uart a/b interrupts enabled */
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
        intEnable |= (1ul<<shift);
#ifndef UART_A_SUPPORT
        intEnable |= BCHP_IRQ0_IRQSTAT_uairq_MASK;
#endif
#ifndef UART_B_SUPPORT
        intEnable |= BCHP_IRQ0_IRQSTAT_ubirq_MASK;
#endif
        BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable );
        break;
    BINT_P_IRQ0_AON_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_AON_ENABLE);
        intEnable |= (1ul<<shift);
        BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_AON_ENABLE, intEnable );
        break;
    BINT_P_UPGSC_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
        intEnable |= 1ul<<shift;
        BREG_Write32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE, intEnable );
        break;
    BINT_P_TIMER_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK );
        intEnable |= (1ul<<shift);
        BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_MASK, intEnable );
        break;

    BINT_P_XPT_MSG_ERR_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
        intEnable |= ( 1ul << shift );
        BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE, intEnable);
        break;

    BINT_P_PCROFFSET_CASES
        intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_PCROFFSET_ENABLE );
        intEnable |= ( 1ul << shift );
        BREG_Write32( regHandle, baseAddr + BINT_P_PCROFFSET_ENABLE, intEnable);
        break;

    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        break;
    }
}

static uint32_t BINT_P_7435_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
    BDBG_MSG(("ReadStatus %#x", baseAddr));
    switch( baseAddr )
    {
    BINT_P_XPT_STATUS_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_STATUS );
    BINT_P_XPT_RAVE_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_RAVE_STATUS );
    BINT_P_XPT_BUF_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_STATUS );
    BINT_P_TIMER_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_STATUS );
    BINT_P_IRQ0_CASES
        /* we need to keep uart a/b interrupts enabled, and mask them for application */
        return (BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS ) 
#ifndef UART_A_SUPPORT
        & ~(BCHP_IRQ0_IRQSTAT_uairq_MASK)
#endif
#ifndef UART_B_SUPPORT
        & ~(BCHP_IRQ0_IRQSTAT_ubirq_MASK)
#endif
        );
    BINT_P_IRQ0_AON_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_AON_STATUS );
    BINT_P_UPGSC_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
    BINT_P_XPT_MSG_ERR_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS );

    BINT_P_PCROFFSET_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_PCROFFSET_STATUS );

    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        return 0;
    }
}

#if 0
static void verify_map(const BINT_P_IntMap *m1, const BINT_P_IntMap *m2)
{
    unsigned i;
    for(i=0;;i++) {
        int m1l1s = m1[i].L1Shift & ~(BINT_P_MAP_MISC_WEAK_MASK | BINT_P_MAP_MISC_STANDARD);
        int m2l1s = m1[i].L1Shift & ~(BINT_P_MAP_MISC_WEAK_MASK | BINT_P_MAP_MISC_STANDARD);
        if(m1l1s != m2l1s || m1[i].L2RegOffset != m2[i].L2RegOffset || m1[i].L2InvalidMask != m2[i].L2InvalidMask) {
            BDBG_WRN(("found mismatch at %u: %d:%d %u:%u %#x:%#x %s:%s", i, m1l1s, m2l1s, m1[i].L2RegOffset, m2[i].L2RegOffset, m1[i].L2InvalidMask, m2[i].L2InvalidMask, m1[i].L2Name, m2[i].L2Name));
        }
        if(m1[i].L1Shift == -1 || m1[i].L1Shift == -1) {
            break;
        }
    }
    return;
}
#endif

const BINT_Settings *BINT_7435_GetSettings( void )
{
    return &bint_7435Settings;
}


static uint32_t GetRaveIntEnableOffset(
    uint32_t BaseAddr
    )
{
    uint32_t EnableAddr = 0;

    switch( BaseAddr )
    {
        case BCHP_XPT_RAVE_INT_CX0: EnableAddr =   BCHP_XPT_RAVE_CX0_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX1: EnableAddr =   BCHP_XPT_RAVE_CX1_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX2: EnableAddr =   BCHP_XPT_RAVE_CX2_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX3: EnableAddr =   BCHP_XPT_RAVE_CX3_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX4: EnableAddr =   BCHP_XPT_RAVE_CX4_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX5: EnableAddr =   BCHP_XPT_RAVE_CX5_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX6: EnableAddr =   BCHP_XPT_RAVE_CX6_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX7: EnableAddr =   BCHP_XPT_RAVE_CX7_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX8: EnableAddr =   BCHP_XPT_RAVE_CX8_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX9: EnableAddr =   BCHP_XPT_RAVE_CX9_AV_INTERRUPT_ENABLES; break;
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
        case BCHP_XPT_RAVE_INT_CX24: EnableAddr =  BCHP_XPT_RAVE_CX24_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX25: EnableAddr =  BCHP_XPT_RAVE_CX25_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX26: EnableAddr =  BCHP_XPT_RAVE_CX26_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX27: EnableAddr =  BCHP_XPT_RAVE_CX27_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX28: EnableAddr =  BCHP_XPT_RAVE_CX28_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX29: EnableAddr =  BCHP_XPT_RAVE_CX29_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX30: EnableAddr =  BCHP_XPT_RAVE_CX30_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX31: EnableAddr =  BCHP_XPT_RAVE_CX31_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX32: EnableAddr =  BCHP_XPT_RAVE_CX32_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX33: EnableAddr =  BCHP_XPT_RAVE_CX33_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX34: EnableAddr =  BCHP_XPT_RAVE_CX34_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX35: EnableAddr =  BCHP_XPT_RAVE_CX35_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX36: EnableAddr =  BCHP_XPT_RAVE_CX36_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX37: EnableAddr =  BCHP_XPT_RAVE_CX37_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX38: EnableAddr =  BCHP_XPT_RAVE_CX38_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX39: EnableAddr =  BCHP_XPT_RAVE_CX39_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX40: EnableAddr =  BCHP_XPT_RAVE_CX40_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX41: EnableAddr =  BCHP_XPT_RAVE_CX41_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX42: EnableAddr =  BCHP_XPT_RAVE_CX42_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX43: EnableAddr =  BCHP_XPT_RAVE_CX43_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX44: EnableAddr =  BCHP_XPT_RAVE_CX44_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX45: EnableAddr =  BCHP_XPT_RAVE_CX45_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX46: EnableAddr =  BCHP_XPT_RAVE_CX46_AV_INTERRUPT_ENABLES; break;
        case BCHP_XPT_RAVE_INT_CX47: EnableAddr =  BCHP_XPT_RAVE_CX47_AV_INTERRUPT_ENABLES; break;

        default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        break;
    }

    return EnableAddr;
}


/* End of file */

