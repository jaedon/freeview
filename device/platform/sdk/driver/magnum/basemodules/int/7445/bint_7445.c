/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7445.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 10/16/12 3:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7445/bint_7445.c $
 * 
 * Hydra_Software_Devel/5   10/16/12 3:11p hongtaoz
 * SW7445-54: added BVNF_8 and 9 for expanded RDC descriptors and
 * triggers; removed BVNF_6 and 7 interrupts which do not go to CPU;
 * added BVNB_1;
 * 
 * Hydra_Software_Devel/4   10/15/12 4:41p hongtaoz
 * SW7445-54: updated bint map;
 * 
 * Hydra_Software_Devel/3   8/6/12 4:54p katrep
 * SW7429-1:first compiler pas
 * 
 * Hydra_Software_Devel/2   7/18/12 9:55a katrep
 * SW7445-1:updates for bringup
 * 
 * Hydra_Software_Devel/1   6/12/12 6:20p katrep
 * SW7445-1:initial checkin
 * 
 **************************************************************************/
#include "bstd.h"
#include "bint_7445.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"

/* Standard L2 stuff */
#include "bchp_aud_inth.h"
#include "bchp_shvd_intr2_0.h"
#include "bchp_shvd_intr2_1.h"
#include "bchp_shvd_intr2_2.h"
#include "bchp_bsp_control_intr2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnb_intr2_1.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_bvnf_intr2_8.h"
#include "bchp_bvnf_intr2_9.h"
#include "bchp_bvnm_intr2_0.h"
#include "bchp_bvnm_intr2_1.h"
#include "bchp_v3d_ctl.h"
#include "bchp_clkgen_intr2.h"
#include "bchp_dvp_hr_intr2.h"
#include "bchp_m2mc_l2.h"
#include "bchp_m2mc1_l2.h"
#include "bchp_hdmi_rx_intr2_0.h"
#include "bchp_hdmi_tx_intr2.h"
#include "bchp_hif_spi_intr2.h"
#include "bchp_hif_intr2.h"
#include "bchp_memc_l2_0.h"
#include "bchp_memc_l2_1_0.h"
#include "bchp_memc_l2_2_0.h"
#include "bchp_memc_l2_1.h"
#include "bchp_memc_l2_1_1.h"
#include "bchp_memc_l2_2_1.h"
#include "bchp_memc_l2_2.h"
#include "bchp_memc_l2_1_2.h"
#include "bchp_memc_l2_2_2.h"
#include "bchp_raaga_dsp_inth.h"
#include "bchp_raaga_dsp_inth_1.h"
#include "bchp_raaga_dsp_fw_inth.h"
#include "bchp_raaga_dsp_fw_inth_1.h"
#include "bchp_sm_l2.h"
#include "bchp_sun_l2.h"
#include "bchp_aon_l2.h"
#include "bchp_aon_pm_l2.h"
/* #include "bchp_uhfr_intr2.h" */
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
#if 0 /* TDB , work on mcpb and m2mdma */
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
#endif
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

BDBG_MODULE(interruptinterface_7445);

#define BINT_P_STD_STATUS       0x00
#define BINT_P_STD_SET          0x04
#define BINT_P_STD_CLEAR        0x08
#define BINT_P_STD_MASK_STATUS  0x0C
#define BINT_P_STD_MASK_SET     0x10
#define BINT_P_STD_MASK_CLEAR   0x14


#define BINT_P_STANDARD_L2_CASES \
    case BCHP_AUD_INTH_R5F_STATUS: \
    case BCHP_SHVD_INTR2_0_CPU_STATUS: \
	case BCHP_SHVD_INTR2_1_CPU_STATUS:\
	case BCHP_SHVD_INTR2_2_CPU_STATUS:\
    case BCHP_BSP_CONTROL_INTR2_CPU_STATUS: \
    case BCHP_BVNB_INTR2_CPU_STATUS: \
    case BCHP_BVNB_INTR2_1_CPU_STATUS: \
    case BCHP_BVNF_INTR2_0_R5F_STATUS: \
    case BCHP_BVNF_INTR2_1_R5F_STATUS: \
    case BCHP_BVNF_INTR2_5_R5F_STATUS: \
    case BCHP_BVNF_INTR2_8_R5F_STATUS: \
    case BCHP_BVNF_INTR2_9_R5F_STATUS: \
    case BCHP_BVNM_INTR2_0_R5F_STATUS: \
    case BCHP_BVNM_INTR2_1_R5F_STATUS: \
    case BCHP_CLKGEN_INTR2_CPU_STATUS: \
    case BCHP_DVP_HR_INTR2_CPU_STATUS: \
    case BCHP_M2MC_L2_CPU_STATUS: \
    case BCHP_M2MC1_L2_CPU_STATUS: \
    case BCHP_HDMI_RX_INTR2_0_CPU_STATUS: \
    case BCHP_HDMI_TX_INTR2_CPU_STATUS: \
    case BCHP_MEMC_L2_0_CPU_STATUS: \
    case BCHP_MEMC_L2_1_0_CPU_STATUS: \
    case BCHP_MEMC_L2_2_0_CPU_STATUS: \
	case BCHP_MEMC_L2_1_CPU_STATUS: \
    case BCHP_MEMC_L2_1_1_CPU_STATUS: \
    case BCHP_MEMC_L2_2_1_CPU_STATUS: \
	case BCHP_MEMC_L2_2_CPU_STATUS: \
    case BCHP_MEMC_L2_1_2_CPU_STATUS: \
    case BCHP_MEMC_L2_2_2_CPU_STATUS: \
    case BCHP_RAAGA_DSP_INTH_HOST_STATUS: \
    case BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS: \
    case BCHP_RAAGA_DSP_INTH_1_HOST_STATUS: \
    case BCHP_RAAGA_DSP_FW_INTH_1_HOST_STATUS: \
    case BCHP_SM_L2_CPU_STATUS: \
	case BCHP_SUN_L2_CPU_STATUS: \
    case BCHP_AON_L2_CPU_STATUS: \
    case BCHP_AON_PM_L2_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
    case BCHP_VICE2_L2_0_CPU_STATUS: \
    case BCHP_VICE2_L2_1_CPU_STATUS: \
    case BCHP_UPG_AUX_INTR2_CPU_STATUS: \
    case BCHP_UPG_AUX_AON_INTR2_CPU_STATUS: \
    case BCHP_SID_L2_CPU_STATUS:

#define BINT_P_V3D_CASES \
    case BCHP_V3D_CTL_INTCTL:

#define BINT_P_V3D_STATUS       0
#define BINT_P_V3D_ENABLE       4
#define BINT_P_V3D_DISABLE      8

#define BINT_P_IRQ0_CASES \
    case BCHP_IRQ0_IRQEN:

#define BINT_P_IRQ0_ENABLE      0
#define BINT_P_IRQ0_STATUS      4


#define BINT_P_IRQ0_AON_CASES \
    case BCHP_IRQ0_AON_IRQEN:

#define BINT_P_IRQ0_AON_ENABLE      0
#define BINT_P_IRQ0_AON_STATUS      4



#if 0 
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

#endif

#define BINT_P_XPT_STATUS           0x00
#define BINT_P_XPT_ENABLE           0x04

#define BINT_P_XPT_STATUS_CASES \
    case BCHP_XPT_BUS_IF_INTR_STATUS_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS2_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS3_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS4_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS5_REG: \
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

static void BINT_P_7445_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7445_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7445_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7445_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7445_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7445_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

#if NEXUS_WEBCPU_core1_server
static const BINT_P_IntMap bint_7445[] =
{
    { BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_M2MC1_CPU_INTR_SHIFT + 96,          BCHP_M2MC1_L2_CPU_STATUS,              0,                   "M2MC1"},
    { -1, 0, 0, NULL}
};
#else
static const BINT_P_IntMap bint_7445[] =
{
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_MEMC1_CPU_INTR_SHIFT,               BCHP_MEMC_L2_1_CPU_STATUS,             0,                   "MEMC1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_MEMC1_CPU_INTR_SHIFT,               BCHP_MEMC_L2_1_1_CPU_STATUS,           0,                   "MEMC1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_MEMC0_CPU_INTR_SHIFT,               BCHP_MEMC_L2_0_CPU_STATUS,             0,                   "MEMC0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_MEMC0_CPU_INTR_SHIFT,               BCHP_MEMC_L2_1_0_CPU_STATUS,           0,                   "MEMC0"},


    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_TX_CPU_INTR_SHIFT,             BCHP_HDMI_TX_INTR2_CPU_STATUS,         0,                   "HDMI_TX"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_RX_0_CPU_INTR_SHIFT,           BCHP_HDMI_RX_INTR2_0_CPU_STATUS,       0,                   "HDMI_RX"},

    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT,                 BCHP_M2MC_L2_CPU_STATUS,               0,                   "M2MC0"},

    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DVP_HR_CPU_INTR_SHIFT,              BCHP_DVP_HR_INTR2_CPU_STATUS,          0,                   "DVP"},

    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_CLKGEN_CPU_INTR_SHIFT,              BCHP_CLKGEN_INTR2_CPU_STATUS,          0,                   "CLK_GEN"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_0_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_0_R5F_STATUS,          0,                   "BVNF_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_8_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_8_R5F_STATUS,          0,                   "BVNF_8"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_9_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_9_R5F_STATUS,          0,                   "BVNF_9"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_1_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_1_R5F_STATUS,          0,                   "BVNF_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_5_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_5_R5F_STATUS,          0,                   "BVNF_5"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNM_0_CPU_INTR_SHIFT,              BCHP_BVNM_INTR2_0_R5F_STATUS,          0,                   "BVNM_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNM_1_CPU_INTR_SHIFT,              BCHP_BVNM_INTR2_1_R5F_STATUS,          0,                   "BVNM_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_0_CPU_INTR_SHIFT,              BCHP_BVNB_INTR2_CPU_STATUS,            0,                   "BVNB_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_1_CPU_INTR_SHIFT,              BCHP_BVNB_INTR2_1_CPU_STATUS,          0,                   "BVNB_1"},

    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BSP_CPU_INTR_SHIFT,                 BCHP_BSP_CONTROL_INTR2_CPU_STATUS,     0,                   "BSP"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AIO_CPU_INTR_SHIFT,                 BCHP_AUD_INTH_R5F_STATUS,              0,                   "AIO"},


  /* { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_PM_CPU_INTR_SHIFT + 32,         BCHP_AON_PM_L2_CPU_STATUS,             0,                   "SYS_PM"}, */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_AON_CPU_INTR_SHIFT + 32,        BCHP_AON_L2_CPU_STATUS,                0,                   "SYS_AON"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_CPU_INTR_SHIFT + 32,            BCHP_SUN_L2_CPU_STATUS,                0,                   "SUN"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SHVD0_0_CPU_INTR_SHIFT + 32,        BCHP_SHVD_INTR2_0_CPU_STATUS,           0,                   "SHVD0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_HVD1_0_CPU_INTR_SHIFT + 32,         BCHP_SHVD_INTR2_1_CPU_STATUS,           0,                   "SHVD0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_HVD2_0_CPU_INTR_SHIFT + 32,         BCHP_SHVD_INTR2_2_CPU_STATUS,           0,                   "SHVD0"},


    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SOFT_MODEM_CPU_INTR_SHIFT + 32,     BCHP_SM_L2_CPU_STATUS,                 0,                   "SM"},

    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SID0_0_CPU_INTR_SHIFT + 32,         BCHP_SID_L2_CPU_STATUS,            0,                   "SID"},

    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RAAGA1_FW_CPU_INTR_SHIFT + 32,      BCHP_RAAGA_DSP_FW_INTH_1_HOST_STATUS,  0,                   "RAAGA1_FW"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RAAGA1_CPU_INTR_SHIFT + 32,         BCHP_RAAGA_DSP_INTH_1_HOST_STATUS,     0,                   "RAAGA1_CPU"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RAAGA_FW_CPU_INTR_SHIFT + 32,       BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS,    0,                   "RAAGA_FW"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RAAGA_CPU_INTR_SHIFT + 32,          BCHP_RAAGA_DSP_INTH_HOST_STATUS,       0,                   "RAAGA_CPU"},
#if 0 /* TBD, covered by new MSPB  */
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB0_INTR,                     0,                   "XPT_PB0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB1_INTR,                     0,                   "XPT_PB1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB2_INTR,                     0,                   "XPT_PB2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB3_INTR,                     0,                   "XPT_PB3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB4_INTR,                     0,                   "XPT_PB4"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB5_INTR,                     0,                   "XPT_PB5"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB6_INTR,                     0,                   "XPT_PB6"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB7_INTR,                     0,                   "XPT_PB7"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB8_INTR,                     0,                   "XPT_PB8"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB9_INTR,                     0,                   "XPT_PB9"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB10_INTR,                    0,                   "XPT_PB10"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB11_INTR,                    0,                   "XPT_PB11"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB12_INTR,                    0,                   "XPT_PB12"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB13_INTR,                    0,                   "XPT_PB13"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB14_INTR,                    0,                   "XPT_PB14"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB15_INTR,                    0,                   "XPT_PB15"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB16_INTR,                    0,                   "XPT_PB16"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB17_INTR,                    0,                   "XPT_PB17"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB18_INTR,                    0,                   "XPT_PB18"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB19_INTR,                    0,                   "XPT_PB19"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB20_INTR,                    0,                   "XPT_PB20"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB21_INTR,                    0,                   "XPT_PB21"},
#endif
	{ BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_00_31,      BINT_DONT_PROCESS_L2,"XPT_OVFL_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_32_63,      BINT_DONT_PROCESS_L2,"XPT_OVFL_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_64_95,      BINT_DONT_PROCESS_L2,"XPT_OVFL_2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_96_127,     BINT_DONT_PROCESS_L2,"XPT_OVFL_3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,	   BCHP_XPT_MSG_DAT_ERR_INTR,             0,                   "XPT_MSG_ERR"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31,   BINT_DONT_PROCESS_L2,"XPT_MSG_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63,   BINT_DONT_PROCESS_L2,"XPT_MSG_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95,   BINT_DONT_PROCESS_L2,"XPT_MSG_2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127,  BINT_DONT_PROCESS_L2,"XPT_MSG_3"},

    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FE_INTR_STATUS0_REG,           0,                   "XPT_FE_STATUS0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FE_INTR_STATUS1_REG,           0,                   "XPT_FE_STATUS1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_IBP_PCC_INTR_STATUS_REG,       0,                   "XPT_IBP_PCC"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_PBP_PCC_INTR_STATUS_REG,       0,                   "XPT_PBP_PCC"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_IBP_SCC_INTR_STATUS_REG,       0,                   "XPT_IBP_SCC"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_PBP_SCC_INTR_STATUS_REG,       0,                   "XPT_PBP_SCC"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_IBP_PSG_PROTOCOL_INTR_STATUS_REG,      0,           "XPT_IBP_PSG"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_PBP_PSG_PROTOCOL_INTR_STATUS_REG,      0,           "XPT_PBP_PSG"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_IBP_TRANSPORT_ERROR_INTR_STATUS_REG,   0,           "XPT_IBP_TRANSPORT_ERROR"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_FE_CPU_INTR_SHIFT + 64,         BCHP_XPT_FULL_PID_PARSER_PBP_TRANSPORT_ERROR_INTR_STATUS_REG,   0,           "XPT_PBP_TRANSPORT_ERROR"},

#ifndef BINT_VICE2_DISABLED
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_VICE2_0_CPU_INTR_SHIFT + 64,   		BCHP_VICE2_L2_0_CPU_STATUS,            0,                   "VICE2_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_VICE2_1_CPU_INTR_SHIFT + 64,   		BCHP_VICE2_L2_1_CPU_STATUS,            0,                   "VICE2_1"},
#endif
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_VEC_CPU_INTR_SHIFT + 64,            BCHP_VIDEO_ENC_INTR2_CPU_STATUS,       0,                   "VEC"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_TMR_CPU_INTR_SHIFT + 64,        BCHP_TIMER_TIMER_IS,                   0,                   "UPG_TMR"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_SPI_CPU_INTR_SHIFT + 64,        BCHP_IRQ0_AON_IRQEN,                   0xFFEFFFFF,          "UPG_SPI"},
	{ BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_SC_CPU_INTR_SHIFT + 64,         BCHP_SCIRQ0_SCIRQEN,                   0,                   "UPG_SC"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_V3D_CPU_INTR_SHIFT + 64,            BCHP_V3D_CTL_INTCTL,                   0,                   "V3D"},
	{ BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_MAIN_AON_CPU_INTR_SHIFT + 64,   BCHP_IRQ0_AON_IRQEN,                   0xFFFFFE1C,          "UPG_MAIN_AON"},
	{ BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_MAIN_CPU_INTR_SHIFT + 64,      BCHP_IRQ0_IRQEN,                       0xFFFFFC00,          "UPG_MAIN"},
	{ BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_BSC_AON_CPU_INTR_SHIFT + 64,    BCHP_IRQ0_AON_IRQEN,                   0xE7FFFFFF,          "UPG_BSC_AON"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_AUX_AON_CPU_INTR_SHIFT + 64,    BCHP_UPG_AUX_AON_INTR2_CPU_STATUS,     0,                   "UPG_AUX_AON"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_BSC_CPU_INTR_SHIFT + 64,        BCHP_IRQ0_IRQEN,                       0xF8FFFFFF,          "UPG_BSC"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_UPG_AUX_CPU_INTR_SHIFT + 64,        BCHP_UPG_AUX_INTR2_CPU_STATUS,         0xFFFF9E00,          "UPG_CTK_UPG_TMON"},



    { BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_MEMC2_CPU_INTR_SHIFT + 96,               BCHP_MEMC_L2_2_CPU_STATUS,             0,                   "MEMC2"},
    { BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_MEMC2_CPU_INTR_SHIFT + 96,               BCHP_MEMC_L2_1_2_CPU_STATUS,           0,                   "MEMC2"},
#if !NEXUS_WEBCPU
/* in webcpu mode, core0 doesn't get M2MC1 L1 */
    { BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_M2MC1_CPU_INTR_SHIFT + 96,          BCHP_M2MC1_L2_CPU_STATUS,              0,                   "M2MC1"},
#endif
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 96,     BCHP_XPT_BUS_IF_INTR_STATUS_REG,       0,                   "XPT"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 96,     BCHP_XPT_BUS_IF_INTR_STATUS2_REG,      0,                   "XPT2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 96,     BCHP_XPT_BUS_IF_INTR_STATUS3_REG,      0,                   "XPT3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 96,     BCHP_XPT_BUS_IF_INTR_STATUS4_REG,      0,                   "XPT4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 96,     BCHP_XPT_BUS_IF_INTR_STATUS5_REG,      0,                   "XPT5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 96,     BCHP_XPT_WAKEUP_INTR_STATUS_REG,       0,                   "XPT_WAKEUP"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX0,                 0,                   "RAV_CX0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX1,                 0,                   "RAV_CX1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX2,                 0,                   "RAV_CX2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX3,                 0,                   "RAV_CX3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX4,                 0,                   "RAV_CX4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX5,                 0,                   "RAV_CX5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX6,                 0,                   "RAV_CX6"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX7,                 0,                   "RAV_CX7"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX8,                 0,                   "RAV_CX8"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX9,                 0,                   "RAV_CX9"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX10,                0,                   "RAV_CX10"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX11,                0,                   "RAV_CX11"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX12,                0,                   "RAV_CX12"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX13,                0,                   "RAV_CX13"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX14,                0,                   "RAV_CX14"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX15,                0,                   "RAV_CX15"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX16,                0,                   "RAV_CX16"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX17,                0,                   "RAV_CX17"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX18,                0,                   "RAV_CX18"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX19,                0,                   "RAV_CX19"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX20,                0,                   "RAV_CX20"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX21,                0,                   "RAV_CX21"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX22,                0,                   "RAV_CX22"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX23,                0,                   "RAV_CX23"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX24,                0,                   "RAV_CX24"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX25,                0,                   "RAV_CX25"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX26,                0,                   "RAV_CX26"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX27,                0,                   "RAV_CX27"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX28,                0,                   "RAV_CX28"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX29,                0,                   "RAV_CX29"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX30,                0,                   "RAV_CX30"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX31,                0,                   "RAV_CX32"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX32,                0,                   "RAV_CX33"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX33,                0,                   "RAV_CX33"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX34,                0,                   "RAV_CX34"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX35,                0,                   "RAV_CX35"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX36,                0,                   "RAV_CX36"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX37,                0,                   "RAV_CX37"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX38,                0,                   "RAV_CX38"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX39,                0,                   "RAV_CX39"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX40,                0,                   "RAV_CX40"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX41,                0,                   "RAV_CX41"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX42,                0,                   "RAV_CX42"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX43,                0,                   "RAV_CX43"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX44,                0,                   "RAV_CX44"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX45,                0,                   "RAV_CX45"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX46,                0,                   "RAV_CX46"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_CX47,                0,                   "RAV_CX47"},
 /* don't process it for now */
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_RAV_CPU_INTR_SHIFT + 96,        BCHP_XPT_RAVE_INT_MISC,                BINT_DONT_PROCESS_L2,"RAV_MISC"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR0_INTR_STATUS_REG,        0,                   "XPT_DPCR0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR1_INTR_STATUS_REG,        0,                   "XPT_DPCR1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR2_INTR_STATUS_REG,        0,                   "XPT_DPCR2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR3_INTR_STATUS_REG,        0,                   "XPT_DPCR3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR4_INTR_STATUS_REG,        0,                   "XPT_DPCR4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR5_INTR_STATUS_REG,        0,                   "XPT_DPCR5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR6_INTR_STATUS_REG,        0,                   "XPT_DPCR6"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR7_INTR_STATUS_REG,        0,                   "XPT_DPCR7"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR8_INTR_STATUS_REG,        0,                   "XPT_DPCR8"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_DPCR9_INTR_STATUS_REG,        0,                   "XPT_DPCR9"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS,  0,                   "XPT_PCROFF0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS,  0,                   "XPT_PCROFF1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS,  0,                   "XPT_PCROFF2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W3_STATUS_XPT_PCR_CPU_INTR_SHIFT + 96,        BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS,  0,                   "XPT_PCROFF3"},



    { -1, 0, 0, NULL}
};
#endif

static const BINT_Settings bint_7445Settings =
{
    BINT_P_7445_SetInt,
    BINT_P_7445_ClearInt,
    BINT_P_7445_SetMask,
    BINT_P_7445_ClearMask,
    BINT_P_7445_ReadMask,
    BINT_P_7445_ReadStatus,
    bint_7445,
    "7445"
};

static void BINT_P_7445_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        BREG_Write32( regHandle, baseAddr + BINT_P_STD_SET, 1ul<<shift);
        break;
    default:
        /* Only standard L2 interrupts support setting of interrupts */
        break;
    }
}

static void BINT_P_7445_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    BDBG_MSG(("ClearInt %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
        BINT_P_STANDARD_L2_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_STD_CLEAR, 1ul<<shift);
            break;
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
        BINT_P_V3D_CASES
            BREG_Write32( regHandle, baseAddr + BINT_P_V3D_STATUS, 1ul<<shift);
            break;
        default:
            /* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
            break;
    }
}

static void BINT_P_7445_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable;

    uint32_t RaveEnReg = 0;

    BDBG_MSG(("SetMask %#x:%d", baseAddr, shift));

    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        BREG_Write32( regHandle, baseAddr + BINT_P_STD_MASK_SET, 1ul<<shift);
        break;
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

    BINT_P_V3D_CASES
        BREG_Write32( regHandle, baseAddr + BINT_P_V3D_DISABLE, 1ul << shift);
        break;

    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        break;
    }
}

static void BINT_P_7445_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable;

    uint32_t RaveEnReg = 0;

    BDBG_MSG(("ClearMask %#x:%d", baseAddr, shift));
    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        BREG_Write32( regHandle, baseAddr + BINT_P_STD_MASK_CLEAR, 1ul<<shift);
        break;
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

    BINT_P_V3D_CASES
        BREG_Write32( regHandle, baseAddr + BINT_P_V3D_ENABLE, 1ul << shift);
        break;

    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        break;
    }
}

static uint32_t BINT_P_7445_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
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
        return ~BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK );
    BINT_P_IRQ0_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE );
    BINT_P_IRQ0_AON_CASES
        return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_AON_ENABLE );
    BINT_P_UPGSC_CASES
        return ~BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
    BINT_P_XPT_MSG_ERR_CASES
        return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );

    BINT_P_PCROFFSET_CASES
        return ~BREG_Read32( regHandle, baseAddr + BINT_P_PCROFFSET_ENABLE );

    BINT_P_V3D_CASES
        return ~BREG_Read32( regHandle, baseAddr + BINT_P_V3D_ENABLE );

        default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        return 0;
    }
}

static uint32_t BINT_P_7445_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
    BDBG_MSG(("ReadStatus %#x", baseAddr));
    switch( baseAddr )
    {
    BINT_P_STANDARD_L2_CASES
        return BREG_Read32(regHandle, baseAddr + BINT_P_STD_STATUS);
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

    BINT_P_V3D_CASES
        return ~BREG_Read32( regHandle, baseAddr + BINT_P_V3D_STATUS );

    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        return 0;
    }
}

const BINT_Settings *BINT_7445_GetSettings( void )
{
    return &bint_7445Settings;
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

