/***************************************************************************
 *     (c)2003-2011 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bint_7584.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 8/22/12 8:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7584/bint_7584.c $
 * 
 * Hydra_Software_Devel/3   8/22/12 8:12p chengs
 * SW7584-4: Update for LEAP.
 * 
 * Hydra_Software_Devel/2   8/22/12 7:59p chengs
 * SW7584-4: Update for LEAP.
 * 
 * Hydra_Software_Devel/1   4/27/12 11:53a chengs
 * SW7584-4: Add int module support for 7584.
 *
 *
 **************************************************************************/
#include "bstd.h"
#include "bint_7584.h"
#include "bchp_7584.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"

/* Standard L2 stuff */
#include "bchp_aio_inth.h"
#include "bchp_avd_intr2_0.h"
#include "bchp_bsp_control_intr2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_bvnm_intr2_0.h"
#include "bchp_hdmi_tx_intr2.h"
#include "bchp_hif_spi_intr2.h"
#include "bchp_hif_intr2.h"
#include "bchp_memc_l2_0.h"
#include "bchp_raaga_dsp_inth.h"
#include "bchp_raaga_dsp_fw_inth.h"
#include "bchp_sm_l2.h"
#include "bchp_sun_l2.h"
#include "bchp_aon_l2.h"
#include "bchp_aon_pm_l2.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_mcif_intr2.h"
#include "bchp_gfx_l2.h"
#include "bchp_clkgen_intr2.h"
#include "bchp_upg_aux_aon_intr2.h"
#include "bchp_ds_topm.h"
#include "bchp_ds_wfe_micro.h"
#include "bchp_oob.h"
#include "bchp_rfm_l2.h"
#include "bchp_sid_l2.h"
#include "bchp_leap_host_l2.h"

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
#include "bchp_xpt_pb4.h"
#include "bchp_xpt_pb5.h"
#include "bchp_xpt_rave.h"
#include "bchp_xpt_msg.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_xpt_wakeup.h"
#include "bchp_xpt_full_pid_parser.h"

/* UARTs, keypad, I2C */
#include "bchp_irq0.h"
#include "bchp_irq0_aon.h"
#include "bchp_upg_aux_aon_intr2.h"

/* Smartcard interrupts. */
#include "bchp_scirq0.h"

/* Timer */
#include "bchp_timer.h"

BDBG_MODULE(interruptinterface_7584);

#define BINT_P_STD_STATUS       0x00
#define BINT_P_STD_SET          0x04
#define BINT_P_STD_CLEAR        0x08
#define BINT_P_STD_MASK_STATUS  0x0C
#define BINT_P_STD_MASK_SET     0x10
#define BINT_P_STD_MASK_CLEAR   0x14


#define BINT_P_STANDARD_L2_CASES \
    case BCHP_AIO_INTH_R5F_STATUS: \
    case BCHP_AVD_INTR2_0_CPU_STATUS: \
    case BCHP_BSP_CONTROL_INTR2_CPU_STATUS: \
    case BCHP_BVNB_INTR2_CPU_STATUS: \
    case BCHP_BVNF_INTR2_0_R5F_STATUS: \
    case BCHP_BVNF_INTR2_1_R5F_STATUS: \
    case BCHP_BVNF_INTR2_3_AVD0_STATUS: \
    case BCHP_BVNF_INTR2_4_AVD0_STATUS: \
    case BCHP_BVNF_INTR2_5_R5F_STATUS: \
    case BCHP_BVNM_INTR2_0_R5F_STATUS: \
    case BCHP_GFX_L2_CPU_STATUS: \
    case BCHP_HIF_INTR2_CPU_STATUS: \
    case BCHP_HDMI_TX_INTR2_CPU_STATUS: \
    case BCHP_MEMC_L2_0_R5F_STATUS: \
    case BCHP_RAAGA_DSP_INTH_HOST_STATUS: \
    case BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS: \
    case BCHP_SM_L2_CPU_STATUS: \
    case BCHP_SUN_L2_CPU_STATUS: \
    case BCHP_AON_L2_CPU_STATUS: \
    case BCHP_AON_PM_L2_CPU_STATUS: \
    case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
    case BCHP_MCIF_INTR2_CPU_STATUS: \
    case BCHP_CLKGEN_INTR2_CPU_STATUS: \
    case BCHP_UPG_AUX_AON_INTR2_CPU_STATUS: \
    case BCHP_SID_L2_CPU_STATUS: \
    case BCHP_DS_TOPM_L2_IRQSTS: \
    case BCHP_DS_WFE_MICRO_IRQSTS: \
    case BCHP_OOB_IRQSTA2C: \
    case BCHP_RFM_L2_CPU_STATUS: \
    case BCHP_LEAP_HOST_L2_CPU_STATUS: \
        
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
    case BCHP_XPT_FE_INTR_STATUS0_REG: \
    case BCHP_XPT_FE_INTR_STATUS1_REG: \
    case BCHP_XPT_DPCR0_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR1_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR2_INTR_STATUS_REG: \
    case BCHP_XPT_DPCR3_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_PCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_PCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_SCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_SCC_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_PSG_PROTOCOL_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_PSG_PROTOCOL_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_IBP_TRANSPORT_ERROR_INTR_STATUS_REG: \
    case BCHP_XPT_FULL_PID_PARSER_PBP_TRANSPORT_ERROR_INTR_STATUS_REG: \
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
    case BCHP_XPT_RAVE_INT_CX23:

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

static void BINT_P_7584_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7584_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7584_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7584_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7584_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7584_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

static const BINT_P_IntMap bint_7584[] =
{
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AIO_CPU_INTR_SHIFT,                 BCHP_AIO_INTH_R5F_STATUS,              0,                   "AIO"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BSP_CPU_INTR_SHIFT,                 BCHP_BSP_CONTROL_INTR2_CPU_STATUS,     0,                   "BSP"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_0_CPU_INTR_SHIFT,              BCHP_BVNB_INTR2_CPU_STATUS,            0,                   "BVNB_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_0_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_0_R5F_STATUS,          0,                   "BVNF_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_1_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_1_R5F_STATUS,          0,                   "BVNF_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_5_CPU_INTR_SHIFT,              BCHP_BVNF_INTR2_5_R5F_STATUS,          0,                   "BVNF_5"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNM_0_CPU_INTR_SHIFT,              BCHP_BVNM_INTR2_0_R5F_STATUS,          0,                   "BVNM"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_CLKGEN_CPU_INTR_SHIFT,              BCHP_CLKGEN_INTR2_CPU_STATUS,          0,                   "CLKGEN"},
    /* EXT_IRQ_0_CPU_INTR - EXT_IRQ_14_CPU_INTR is not mapped */
    /* GENET_0_A_CPU_INTR, GENET_0_B_CPU_INTR is not mapped */
    /* GENET_1_A_CPU_INTR, GENET_1_B_CPU_INTR is not mapped */
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT,                 BCHP_GFX_L2_CPU_STATUS,                0,                   "GFX"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_TX_CPU_INTR_SHIFT,             BCHP_HDMI_TX_INTR2_CPU_STATUS,         0,                   "HDMI_TX"},
    /* HIF_CPU_INTR is not mapped */
    /* HIF_SPI_CPU_INTR is not mapped */
    

    /* IPI0_CPU_INTR, IPI1_CPU_INTR is not mapped */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_MEMC_0_CPU_INTR_SHIFT + 32,         BCHP_MEMC_L2_0_R5F_STATUS,             0,                   "MEMC0"},
    /* NMI_PIN_CPU_INTR is not mapped */
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RAAGA_CPU_INTR_SHIFT + 32,          BCHP_RAAGA_DSP_INTH_HOST_STATUS,       0,                   "RAAGA_CPU"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RAAGA_FW_CPU_INTR_SHIFT + 32,       BCHP_RAAGA_DSP_FW_INTH_HOST_STATUS,    0,                   "RAAGA_FW"},
    
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SID_CPU_INTR_SHIFT + 32,            BCHP_SID_L2_CPU_STATUS,                0,                   "SID"},
    
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SOFT_MODEM_CPU_INTR_SHIFT + 32,     BCHP_SM_L2_CPU_STATUS,                 0,                   "SM"},    
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_CPU_INTR_SHIFT + 32,            BCHP_SUN_L2_CPU_STATUS,                0,                   "SUN"},    
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_AON_CPU_INTR_SHIFT + 32,        BCHP_AON_L2_CPU_STATUS,                0,                   "SYS_AON"},
    /*BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_PM_CPU_INTR_SHIFT + 32,         BCHP_AON_PM_L2_CPU_STATUS,             0,                   "SYS_PM"},*/
    
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_AUX_AON_CPU_INTR_SHIFT + 32,    BCHP_UPG_AUX_AON_INTR2_CPU_STATUS,     0,                   "UPG_AUX_AON"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_BSC_CPU_INTR_SHIFT + 32,        BCHP_IRQ0_IRQEN,                       0xF8FFFFFF,          "UPG_BSC"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_BSC_AON_CPU_INTR_SHIFT + 32,    BCHP_IRQ0_AON_IRQEN,                   0xF7FFFFFF,          "UPG_BSC_AON"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_MAIN_CPU_INTR_SHIFT + 32,       BCHP_IRQ0_IRQEN,                       0xFFFFFC00,          "UPG_MAIN"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_MAIN_AON_CPU_INTR_SHIFT + 32,   BCHP_IRQ0_AON_IRQEN,                   0xFFFFFC00,          "UPG_MAIN_AON"},    
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_SC_CPU_INTR_SHIFT + 32,         BCHP_SCIRQ0_SCIRQEN,                   0,                   "UPG_SC"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_SPI_CPU_INTR_SHIFT + 32,        BCHP_IRQ0_AON_IRQEN,                   0xFFEFFFFF,          "UPG_SPI"},
    { BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_TMR_CPU_INTR_SHIFT + 32,        BCHP_TIMER_TIMER_IS,                   0,                   "UPG_TMR"},


    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_VEC_CPU_INTR_SHIFT + 64,            BCHP_VIDEO_ENC_INTR2_CPU_STATUS,       0,                   "VEC"},
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
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31,   BINT_DONT_PROCESS_L2,"XPT_MSG_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63,   BINT_DONT_PROCESS_L2,"XPT_MSG_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95,   BINT_DONT_PROCESS_L2,"XPT_MSG_2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_CPU_INTR_SHIFT + 64,        BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127,  BINT_DONT_PROCESS_L2,"XPT_MSG_3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_MSG_STAT_CPU_INTR_SHIFT + 64,   BCHP_XPT_MSG_DAT_ERR_INTR,             0,                   "XPT_MSG_ERR"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_OVFL_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_00_31,      BINT_DONT_PROCESS_L2,"XPT_OVFL_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_OVFL_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_32_63,      BINT_DONT_PROCESS_L2,"XPT_OVFL_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_OVFL_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_64_95,      BINT_DONT_PROCESS_L2,"XPT_OVFL_2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_OVFL_CPU_INTR_SHIFT + 64,       BCHP_XPT_MSG_BUF_OVFL_INTR_96_127,     BINT_DONT_PROCESS_L2,"XPT_OVFL_3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_DPCR0_INTR_STATUS_REG,        0,                   "XPT_DPCR0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_DPCR1_INTR_STATUS_REG,        0,                   "XPT_DPCR1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_DPCR2_INTR_STATUS_REG,        0,                   "XPT_DPCR2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_DPCR3_INTR_STATUS_REG,        0,                   "XPT_DPCR3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS,  0,                   "XPT_PCROFF0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS,  0,                   "XPT_PCROFF1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_PCROFFSET_INTERRUPT2_STATUS,  0,                   "XPT_PCROFF2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_PCR_CPU_INTR_SHIFT + 64,        BCHP_XPT_PCROFFSET_INTERRUPT3_STATUS,  0,                   "XPT_PCROFF3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX0,                 0,                   "RAV_CX0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX1,                 0,                   "RAV_CX1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX2,                 0,                   "RAV_CX2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX3,                 0,                   "RAV_CX3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX4,                 0,                   "RAV_CX4"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX5,                 0,                   "RAV_CX5"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX6,                 0,                   "RAV_CX6"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX7,                 0,                   "RAV_CX7"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX8,                 0,                   "RAV_CX8"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX9,                 0,                   "RAV_CX9"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX10,                0,                   "RAV_CX10"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX11,                0,                   "RAV_CX11"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX12,                0,                   "RAV_CX12"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX13,                0,                   "RAV_CX13"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX14,                0,                   "RAV_CX14"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX15,                0,                   "RAV_CX15"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX16,                0,                   "RAV_CX16"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX17,                0,                   "RAV_CX17"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX18,                0,                   "RAV_CX18"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX19,                0,                   "RAV_CX19"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX20,                0,                   "RAV_CX20"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX21,                0,                   "RAV_CX21"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX22,                0,                   "RAV_CX22"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_CX23,                0,                   "RAV_CX23"},
    /*{ BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_RAV_CPU_INTR_SHIFT + 64,        BCHP_XPT_RAVE_INT_MISC,                0,                   "RAV_MISC"},*/
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_BUS_IF_INTR_STATUS_REG,       0,                   "XPT"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_BUS_IF_INTR_STATUS2_REG,      0,                   "XPT2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_BUS_IF_INTR_STATUS3_REG,      0,                   "XPT3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_BUS_IF_INTR_STATUS4_REG,      0,                   "XPT4"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_BUS_IF_INTR_STATUS5_REG,      0,                   "XPT5"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB0_INTR,                     0,                   "XPT_PB0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB1_INTR,                     0,                   "XPT_PB1"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB2_INTR,                     0,                   "XPT_PB2"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB3_INTR,                     0,                   "XPT_PB3"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB4_INTR,                     0,                   "XPT_PB4"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_PB5_INTR,                     0,                   "XPT_PB5"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_XPT_STATUS_CPU_INTR_SHIFT + 64,     BCHP_XPT_WAKEUP_INTR_STATUS_REG,       0,                   "XPT_WAKEUP"},
    /* SDIO0_0_CPU_INTR is not mapped */
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_AVD0_0_CPU_INTR_SHIFT + 64,         BCHP_AVD_INTR2_0_CPU_STATUS,           0,                   "AVD0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_RFM_CPU_INTR_SHIFT + 64,            BCHP_RFM_L2_CPU_STATUS,                0,                   "RFM"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_DS_0_WFE_CPU_INTR_SHIFT + 64,       BCHP_DS_WFE_MICRO_IRQSTS,              0,                   "DS_WFE"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_DS_0_CPU_INTR_SHIFT + 64,           BCHP_DS_TOPM_L2_IRQSTS,                0,                   "DS_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_DS_0_OOB_0_CPU_INTR_SHIFT + 64,     BCHP_OOB_IRQSTA2C,                     0,                   "OOB_0"},    
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_DS_0_OOB_1_CPU_INTR_SHIFT + 64,     BCHP_OOB_IRQSTA2C,                     0,                   "OOB_1  "},    
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_MCIF_CPU_INTR_SHIFT + 64,           BCHP_MCIF_INTR2_CPU_STATUS,            0,                   "UPG_AUX"}, 
    { BCHP_HIF_CPU_INTR1_INTR_W2_STATUS_LEAP_CPU_INTR_SHIFT + 64,           BCHP_LEAP_HOST_L2_CPU_STATUS,          0,                   "LEAP"},
    { -1, 0, 0, NULL}
};

static const BINT_Settings bint_7584Settings =
{
    BINT_P_7584_SetInt,
    BINT_P_7584_ClearInt,
    BINT_P_7584_SetMask,
    BINT_P_7584_ClearMask,
    BINT_P_7584_ReadMask,
    BINT_P_7584_ReadStatus,
    bint_7584,
    "7584"
};

static void BINT_P_7584_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    /*uint16_t ulChipID = BCHP_GetChipID();*/

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

static void BINT_P_7584_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    /*uint16_t ulChipID = BCHP_GetChipID();*/

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
        default:
            /* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
            break;
    }
}

static void BINT_P_7584_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable;

    uint32_t RaveEnReg = 0;
    /*uint16_t ulChipID = BCHP_GetChipID();*/

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

static void BINT_P_7584_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
    uint32_t intEnable;

    uint32_t RaveEnReg = 0;
    /*uint16_t ulChipID = BCHP_GetChipID();*/

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

static uint32_t BINT_P_7584_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
{
    uint32_t RaveEnReg = 0;
    /*uint16_t ulChipID = BCHP_GetChipID();*/

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
    default:
        /* Unhandled interrupt base address */
        BDBG_ASSERT( false );
        return 0;
    }
}

static uint32_t BINT_P_7584_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
    /*uint16_t ulChipID = BCHP_GetChipID();*/

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
        return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS );
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

const BINT_Settings *BINT_7584_GetSettings( void )
{
    return &bint_7584Settings;
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

