/***************************************************************************
 *	   Copyright (c) 2003-2012, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
  *
 * $brcm_Workfile: bint_7344.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 10/5/12 11:13a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7344/bint_7344.c $
 * 
 * Hydra_Software_Devel/10   10/5/12 11:13a mphillip
 * SW7425-3963: Merge interrupt changes to main
 * 
 * Hydra_Software_Devel/SW7425-3963/1   9/27/12 5:42p mphillip
 * SW7425-3963: Update interrupt handling with optimizations and support
 * for 'weak' interrupt masking
 * 
 * Hydra_Software_Devel/9   3/22/12 3:01p randyjew
 * SW7418-84: Modify Interrupt Map for TMON and CTK
 * 
 * Hydra_Software_Devel/8   3/17/12 7:53p agin
 * SWNOOS-518:  Support BICAP.
 * 
 * Hydra_Software_Devel/7   1/30/12 6:04p randyjew
 * SW7344-247:Remove outdated UPG Smartcard interrupt implementation
 * 
 * Hydra_Software_Devel/6   12/13/11 4:18p randyjew
 * SW7344-246: remove masked interrupts for Uart A/B
 * 
 * Hydra_Software_Devel/5   12/6/11 6:13p randyjew
 * SW7344-220: Do not enable SYS_PM interrupt (SW7231-506)
 * 
 * Hydra_Software_Devel/4   12/14/10 11:49a jrubio
 * SW7344-9: add sds interrupts
 * 
 * Hydra_Software_Devel/3   12/8/10 3:50p jrubio
 * SW7344-9: update int file
 * 
 * Hydra_Software_Devel/2   11/19/10 10:54a jrubio
 * SW7344-9: update bint
 * 
 * Hydra_Software_Devel/1   11/16/10 11:36a jrubio
 * SW7344-9: add 7344 support
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bint_7344.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"

/* Standard L2 stuff */
#include "bchp_aio_inth.h"
#include "bchp_bsp_control_intr2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_bvnm_intr2_0.h"
#include "bchp_clkgen_intr2.h"
#include "bchp_gfx_l2.h"
#include "bchp_hdmi_tx_intr2.h"
#include "bchp_hif_spi_intr2.h"
#include "bchp_hif_intr2.h"
#include "bchp_memc_l2_0.h"
#include "bchp_raaga_dsp_inth.h"
#include "bchp_raaga_dsp_fw_inth.h"
#include "bchp_sm_l2.h"
#include "bchp_svd_intr2_0.h"
#include "bchp_sun_l2.h"
#include "bchp_aon_l2.h"
#include "bchp_aon_pm_l2.h"
#include "bchp_uhfr_intr2.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_upg_aux_intr2.h"
#include "bchp_upg_aux_aon_intr2.h"

/* XPT */
#include "bchp_xpt_fe.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bchp_xpt_dpcr2.h"
#include "bchp_xpt_dpcr3.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"
#include "bchp_xpt_pb2.h"
#include "bchp_xpt_pb3.h"
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

/* SDS */
#include "bchp_afec_intr_ctrl2.h"
#include "bchp_sds_intr2_0.h"
#include "bchp_sds_intr2_1.h"
#include "bchp_tfec_intr2.h"
#include "bchp_ftm_intr2.h"

BDBG_MODULE(interruptinterface_7344);

#define BINT_P_STD_STATUS		0x00
#define BINT_P_STD_SET			0x04
#define BINT_P_STD_CLEAR		0x08
#define BINT_P_STD_MASK_STATUS	0x0C
#define BINT_P_STD_MASK_SET		0x10
#define BINT_P_STD_MASK_CLEAR	0x14


#define	BINT_P_IRQ0_CASES \
	case BCHP_IRQ0_IRQEN:

#define BINT_P_IRQ0_ENABLE		0
#define BINT_P_IRQ0_STATUS		4


#define	BINT_P_IRQ0_AON_CASES \
	case BCHP_IRQ0_AON_IRQEN:

#define BINT_P_IRQ0_AON_ENABLE		0
#define BINT_P_IRQ0_AON_STATUS		4


#define BINT_P_XPT_STATUS			0x00
#define BINT_P_XPT_ENABLE			0x04

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
	case BCHP_XPT_FE_INTR_STATUS0_REG: \
	case BCHP_XPT_FE_INTR_STATUS1_REG: \
	case BCHP_XPT_DPCR0_INTR_STATUS_REG: \
	case BCHP_XPT_DPCR1_INTR_STATUS_REG: \
	case BCHP_XPT_DPCR2_INTR_STATUS_REG: \
	case BCHP_XPT_DPCR3_INTR_STATUS_REG: \
	case BCHP_XPT_WAKEUP_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_IBP_PCC_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_PBP_PCC_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_IBP_SCC_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_PBP_SCC_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_IBP_PSG_PROTOCOL_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_PBP_PSG_PROTOCOL_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_IBP_TRANSPORT_ERROR_INTR_STATUS_REG: \
	case BCHP_XPT_FULL_PID_PARSER_PBP_TRANSPORT_ERROR_INTR_STATUS_REG: 

#define BINT_P_PCROFFSET_CASES \
	case BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS: \
	case BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS: \
	case BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS: \
	case BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS:

#define BINT_P_PCROFFSET_STATUS		0x00
#define BINT_P_PCROFFSET_ENABLE 	0x04

/* There is no constant address mapping from RAVE status to RAVE enable registers. */
#define BINT_P_RAVE_STATUS			0x00

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

#define BINT_P_XPT_BUF_STATUS		0x00
#define BINT_P_XPT_BUF_ENABLE		0x10

#define BINT_P_XPT_BUF_CASES \
	case BCHP_XPT_MSG_BUF_OVFL_INTR_00_31: \
	case BCHP_XPT_MSG_BUF_OVFL_INTR_32_63: \
	case BCHP_XPT_MSG_BUF_OVFL_INTR_64_95: \
	case BCHP_XPT_MSG_BUF_OVFL_INTR_96_127: \
	case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31: \
	case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63: \
	case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95: \
	case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127:

#define BINT_P_XPT_MSG_ERR_STATUS	( 0x00 )
#define BINT_P_XPT_MSG_ERR_ENABLE	( 0x04 )
#define BINT_P_XPT_MSG_ERR_CASES \
	case BCHP_XPT_MSG_DAT_ERR_INTR:

/* BINT_P_UPGSC_ENABLE was defined as -4 for BCHP_SCIRQ0_SCIRQSTAT.
 * Since we are using BCHP_SCIRQ0_SCIRQEN, it is not needed but
 * to minimize the change, it is kept and set to 0
 */
#define BINT_P_UPGSC_ENABLE (0)

#define BINT_P_UPGSC_CASES \
	case BCHP_SCIRQ0_SCIRQEN:

#define BINT_P_TIMER_STATUS		0x00
#define BINT_P_TIMER_MASK		0x04

#define BINT_P_TIMER_CASES \
	case BCHP_TIMER_TIMER_IS:

#define BINT_P_STAT_TIMER_TICKS_PER_USEC 27

static void BINT_P_7344_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7344_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7344_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7344_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

static const BINT_P_IntMap bint_7344[] =
{
    BINT_MAP_STD(1, MEMC0, MEMC_L2_0_R5F),

    BINT_MAP_STD(1, HDMI_TX, HDMI_TX_INTR2_CPU),
	
    BINT_MAP_STD(1, GFX, GFX_L2_CPU),

	
    BINT_MAP_STD(0, CLKGEN, CLKGEN_INTR2_CPU),

    BINT_MAP_STD(0, BVNM_0, BVNM_INTR2_0_R5F),
    BINT_MAP_STD(0, BVNF_5, BVNF_INTR2_5_R5F),
    BINT_MAP_STD(0, BVNF_1, BVNF_INTR2_1_R5F),
    BINT_MAP_STD(0, BVNF_0, BVNF_INTR2_0_R5F),
    BINT_MAP_STD(0, BVNB_0, BVNB_INTR2_CPU),

    BINT_MAP_STD(0, FTM, FTM_INTR2_CPU),

    BINT_MAP_STD(0, BSP, BSP_CONTROL_INTR2_CPU),

    /* BINT_MAP_STD(0, AVD1_0, AVD_INTR2_1_CPU),*/

    BINT_MAP_STD(0, AIO, AIO_INTH_R5F),

    /*BINT_MAP(1, UPG_AUX, "_CTK", UPG_AUX_INTR2_CPU_STATUS, STANDARD, MASK, 0xFFFFFE00),*/
    BINT_MAP(1, UPG_AUX, "_TMON", UPG_AUX_INTR2_CPU_STATUS, STANDARD, MASK, 0xFFFF9FFF),

    BINT_MAP(1, UPG_TMR, "" , TIMER_TIMER_IS, REGULAR, ALL, 0),

    BINT_MAP(1, UPG_SPI, "" , IRQ0_AON_IRQEN, REGULAR, MASK, 0xFFEFFFFF),

    BINT_MAP(1, UPG_SC, "", SCIRQ0_SCIRQEN, REGULAR, ALL, 0),

    BINT_MAP(1, UPG_MAIN, "" , IRQ0_IRQEN, REGULAR, MASK, 0xFFFFFC00),
    BINT_MAP(1, UPG_BSC, "" , IRQ0_IRQEN, REGULAR, MASK, 0xF8FFFFFF),

    BINT_MAP(1, UPG_BSC_AON, "" , IRQ0_AON_IRQEN, REGULAR, MASK, 0xE7FFFFFF),

    BINT_MAP(1, UPG_MAIN_AON, "" , IRQ0_AON_IRQEN, REGULAR, MASK, 0xFFFFFE1C),

    BINT_MAP(1, UPG_AUX_AON, "", UPG_AUX_AON_INTR2_CPU_STATUS, STANDARD, ALL, 0),

    BINT_MAP_STD(1, UHF, UHFR_INTR2_CPU),

    /* BINT_MAP_STD(1, SYS_PM, AON_PM_L2_CPU), */
    BINT_MAP_STD(1, SYS_AON, AON_L2_CPU),
    BINT_MAP_STD(1, SYS, SUN_L2_CPU),

    BINT_MAP_STD(1, SVD0_0, SVD_INTR2_0_CPU),

    BINT_MAP_STD(1, SOFT_MODEM, SM_L2_CPU),

    BINT_MAP_STD(1, SDS0_TFEC, TFEC_INTR2_CPU),
    BINT_MAP_STD(1, SDS0_RCVR_1, SDS_INTR2_1_CPU),
    BINT_MAP_STD(1, SDS0_RCVR_0, SDS_INTR2_0_CPU),
    BINT_MAP_STD(1, SDS0_AFEC, AFEC_INTR_CTRL2_CPU),

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

    BINT_MAP(2, XPT_PCR, "0", XPT_DPCR0_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "1", XPT_DPCR1_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "2", XPT_DPCR2_INTR_STATUS_REG, REGULAR, ALL, 0),
    BINT_MAP(2, XPT_PCR, "3", XPT_DPCR3_INTR_STATUS_REG, REGULAR, ALL, 0),
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

    BINT_MAP_STD(2, VEC, VIDEO_ENC_INTR2_CPU),

	{ -1, 0, 0, NULL}
};

static const BINT_Settings bint_7344Settings =
{
    NULL,
    BINT_P_7344_ClearInt,
    BINT_P_7344_SetMask,
    BINT_P_7344_ClearMask,
    NULL,
    BINT_P_7344_ReadStatus,
    bint_7344,
    "7344"
};

static void BINT_P_7344_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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

static void BINT_P_7344_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
		intEnable &= ~(1ul<<shift);
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

static void BINT_P_7344_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
		intEnable |= (1ul<<shift);
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

static uint32_t BINT_P_7344_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
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
		return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS ) & ~(BCHP_IRQ0_IRQSTAT_uairq_MASK
#ifndef UART_B_SUPPORT
		|BCHP_IRQ0_IRQSTAT_ubirq_MASK
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

const BINT_Settings *BINT_7344_GetSettings( void )
{
	return &bint_7344Settings;
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

		default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}

	return EnableAddr;
}


/* End of file */

