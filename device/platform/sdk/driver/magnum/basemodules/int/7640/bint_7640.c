/***************************************************************************
 *	   Copyright (c) 2003-2011, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7640.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 9/22/11 3:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7640/bint_7640.c $
 * 
 * Hydra_Software_Devel/10   9/22/11 3:56p jzhu
 * SWBLURAY-27236:M2MC interrupt change
 * 
 * Hydra_Software_Devel/bdvd_v4.0/2   9/22/11 12:02p jzhu
 * SWBLURAY-27236:[ see Broadcom Issue Tracking JIRA for more info ]
 *
 * Hydra_Software_Devel/9   3/15/11 4:47p etrudeau
 * SWBLURAY-24803: Add case for AON_L2_CPU_STATUS
 * 
 * Hydra_Software_Devel/8   3/15/11 3:58p etrudeau
 * SWBLURAY-24803: Fix L1_bit to L2_register mapping of some AON (e.g.
 * CEC) interrupts
 * 
 * Hydra_Software_Devel/7   3/7/11 5:26p etrudeau
 * SWBLURAY-24951: add offset of 32 to Level 1 W1 Interrupt status shifts
 * 
 * Hydra_Software_Devel/6   3/5/11 6:10p etrudeau
 * SWBLURAY-23688: add UPG_IRQ0_AON_IRQEN Interrupt
 * 
 * Hydra_Software_Devel/5   3/3/11 4:22p etrudeau
 * SWBLURAY-23688: add UPG_BSC_AON Interrupt
 * 
 * Hydra_Software_Devel/4   3/2/11 8:08p etrudeau
 * SWBLURAY-23688: add AON L2 Interrupt Status register
 * 
 * Hydra_Software_Devel/3   3/2/11 6:48p etrudeau
 * SWBLURAY-23688: add AON L2 Interrupt Status register
 * 
 * Hydra_Software_Devel/2   12/10/10 4:49p etrudeau
 * SWBLURAY-23579: update Blast interrupt definitions from Grain diffs
 * 
 * Hydra_Software_Devel/1   12/3/10 12:59p etrudeau
 * SWBLURAY-23579: add 7640 chip
 * 
 * Hydra_Software_Devel/1   12/3/10 12:52p etrudeau
 * SWBLURAY-23579: merge 7630 int files to Hydra
 * 
 * bdvd_v3.0/7   5/21/10 3:27p etrudeau
 * ISSUE19763[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * These changes allow 7631 A0/B0 builds to correctly not use the removed
 * middle I2C Master B channel.
 * Also, the XPT interrupt changes for 7631B0 were affecting 7631A0 and
 * they should not have.
 * 
 * Reviewed by:
 * Ted Hermanson
 * 
 * bdvd_v3.0/6   5/18/10 1:35p etrudeau
 * ISSUE19763[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * Changes needed to allow app_bringup to build successfully for PI
 * appframework development to proceed with support for Quick chip.
 * 
 * bdvd_v3.0/5   4/12/10 9:50a acooper
 * ISSUE19670[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * General_Comments:
 *        Magnum interrupt handler is incorrectly masking some interrupts
 * owned
 *        by the kernel causing the new UDMA serial port driver to hang.
 * 
 *        The "bint_7630A0" table in bint_7630.c includes a L2 mask value
 *        that specifies which L2 interrupt bits belong to the Magnum
 * interrupt
 *        software. If the bit specifies that Magnum owns the interrupt,
 * the
 *        actual L2 interrupt bits will be initialized when Magnum starts
 * up.
 *        The L2 interrupt bits in BCHP_HIF_INTR2_CPU_STATUS for the new
 * UDMA
 *        engine were specified as owned by Magnum and when app_player
 *        started it would hang any active UDMA data transfers. This
 * change
 *        sets the bit mask correctly.
 *        NOTE: The bit mask for the second SHARF DMA engine which should
 * be
 *        owned by the kernel is also set incorrectly, but this will be
 * fixed
 *        in another PR because the crypto code also needs to be changed.
 * 
 * 
 * 
 * File_Specific_Comments:
 * 
 * 
 * Reviewed_By:
 *        Eric Trudeau, Jon Fraser
 * 
 * 
 * Unit_Testing_Performed:
 *        I started a UDMA loopback test and then started app_player and
 *        verified that the data transfer did not hang. I also tested a
 * few
 *        titles with app_player to verify it was still working
 * correctly.
 * 
 * 
 * Review_Comments:
 *        Eric Trudeau
 *        Al, could you verify whether there were any application
 * callbacks
 *        registered for that level 2 interrupt?   In other words, how
 * confident
 *        are you that the magnum code does not actually need that L2
 * IRQ?
 * 
 *        Response:
 *        Registering the callback will now fail, I know this because I
 * also
 *        tried to fix the SHARF DMA interrupt mask and app_player
 * crashed
 *        because it incorrectly tried to register the interrupt. Plus
 * the app
 *        would have no possible use for the UDMA Uart interrupts.
 * 
 * bdvd_v3.0/fix_bdvd_v3.0_issue19670/1   4/9/10 4:01p acooper
 * Fix interrupt bitmask for BCHP_HIF_INTR2_CPU_STATUS which was
 * indicating that the UDMA interrupts were owned by magnum when the
 * interrupts are actually owned by kernel via the UDMA kernel mode
 * driver.
 * 
 * bdvd_v3.0/4   11/6/09 5:09p jquinlan
 * PR17592[DVD]:[ see HiDef-DVD bug tracking system for more info ].
 * 
 * bdvd_v3.0/dev_bdvd_v3.0_pr16694/1   10/27/09 3:53p mpeteri
 * Changes for bdvd_record and messaging on GRAIN.
 * 
 * bdvd_v3.0/3   9/21/09 4:54p jquinlan
 * Fixed typo concerning commented statement.
 * 
 * bdvd_v3.0/2   8/31/09 2:54p jquinlan
 * PR 16362 -- grain (7630) bringup.
 *
 * bdvd_v3.0/1   8/28/09 11:51a jquinlan
 * new file.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bint_7640.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"

/* Standard L2 stuff */
#include "bchp_aio_inth.h"
#include "bchp_aud_dsp_inth0.h"
#include "bchp_aud_dsp_inth0_1.h"
#include "bchp_avd_intr2_0.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#if (BCHP_CHIP==7640 && BCHP_VER == BCHP_VER_B0)
#include "bchp_wrap_m2mc_l2.h"
#else
#include "bchp_gfx_l2.h"
#endif
#include "bchp_hdmi_rx_intr2_0.h"
#include "bchp_hdmi_tx_intr2.h"
#include "bchp_dvp_hr_intr2.h"
#include "bchp_hif_intr2.h"
#include "bchp_sun_l2.h"
#include "bchp_usb_intr2.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_bsp_control_intr2.h"
/*#include "bchp_shift_intr2.h"*/
#include "bchp_fgx_to_hif_intr2.h"
#include "bchp_aon_pm_l2.h"
#include "bchp_aon_l2.h"
#include "bchp_irq0_aon.h"

/* XPT */
#include "bchp_xpt_fe.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"
#include "bchp_xpt_pb2.h"
#include "bchp_xpt_pb3.h"
#include "bchp_xpt_rave.h"
#include "bchp_xpt_pcroffset.h"
#include "bchp_xpt_msg.h"

/* UARTs, keypad, I2C */
#include "bchp_irq0.h"

/* Timer */
#include "bchp_timer.h"

BDBG_MODULE(interruptinterface_7640);

#define BINT_P_STD_STATUS		0x00
#define BINT_P_STD_SET			0x04
#define BINT_P_STD_CLEAR		0x08
#define BINT_P_STD_MASK_STATUS	0x0C
#define BINT_P_STD_MASK_SET		0x10
#define BINT_P_STD_MASK_CLEAR	0x14

/* in 7640 b0, graphics use BCHP_WRAP_M2MC_L2_CPU_STATUS instead of BCHP_GFX_L2_CPU_STATUS */
#if (BCHP_CHIP==7640 && BCHP_VER == BCHP_VER_B0)
#define BINT_P_STANDARD_L2_CASES_BASE \
	case BCHP_AIO_INTH_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_1_R5F_STATUS: \
	case BCHP_AVD_INTR2_0_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_3_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_4_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_WRAP_M2MC_L2_CPU_STATUS:\
	case BCHP_HDMI_TX_INTR2_CPU_STATUS: \
	case BCHP_HDMI_RX_INTR2_0_CPU_STATUS: \
        case BCHP_DVP_HR_INTR2_CPU_STATUS:   \
	case BCHP_HIF_INTR2_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BSP_CONTROL_INTR2_CPU_STATUS: \
	case BCHP_SUN_L2_CPU_STATUS: \
	case BCHP_FGX_TO_HIF_INTR2_CPU_STATUS: \
        case BCHP_AON_PM_L2_CPU_STATUS: \
        case BCHP_AON_L2_CPU_STATUS:
#else
#define BINT_P_STANDARD_L2_CASES_BASE \
	case BCHP_AIO_INTH_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_1_R5F_STATUS: \
	case BCHP_AVD_INTR2_0_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_3_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_4_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_HDMI_TX_INTR2_CPU_STATUS: \
	case BCHP_HDMI_RX_INTR2_0_CPU_STATUS: \
        case BCHP_DVP_HR_INTR2_CPU_STATUS:   \
	case BCHP_HIF_INTR2_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BSP_CONTROL_INTR2_CPU_STATUS: \
	case BCHP_SUN_L2_CPU_STATUS: \
	case BCHP_FGX_TO_HIF_INTR2_CPU_STATUS: \
        case BCHP_AON_PM_L2_CPU_STATUS: \
        case BCHP_AON_L2_CPU_STATUS:
#endif

#define BINT_P_STANDARD_L2_CASES \
	BINT_P_STANDARD_L2_CASES_BASE

#define	BINT_P_IRQ0_CASES \
	case BCHP_IRQ0_IRQEN: \
        case BCHP_IRQ0_AON_IRQEN:

#define BINT_P_IRQ0_ENABLE		0
#define BINT_P_IRQ0_STATUS		4


#define BINT_P_XPT_STATUS			0x00
#define BINT_P_XPT_ENABLE			0x04
#define BINT_P_XPT_FE_ENABLE			0x08

#define BINT_P_XPT_STATUS_CASES \
    case BCHP_XPT_BUS_IF_INTR_STATUS_REG: \
    case BCHP_XPT_BUS_IF_INTR_STATUS2_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS4_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS5_REG: \
	case BCHP_XPT_PB0_INTR: \
	case BCHP_XPT_PB1_INTR: \
    case BCHP_XPT_PB2_INTR: \
    case BCHP_XPT_PB3_INTR: \
	case BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS: \
	case BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95: \
    case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_00_31: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_32_63: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_64_95: \
    case BCHP_XPT_MSG_BUF_OVFL_INTR_96_127:

#define BINT_P_XPT_FE_STATUS_CASES \
    case BCHP_XPT_FE_INTR_STATUS0_REG: \
    case BCHP_XPT_FE_INTR_STATUS1_REG:

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

#define BINT_P_XPT_MSG_ERR_STATUS	( 0x00 )
#define BINT_P_XPT_MSG_ERR_ENABLE	( 0x04 )

/* BINT_P_UPGSC_ENABLE was defined as -4 for BCHP_SCIRQ0_SCIRQSTAT.
 * Since we are using BCHP_SCIRQ0_SCIRQEN, it is not needed but
 * to minimize the change, it is kept and set to 0
 */
#define BINT_P_UPGSC_ENABLE (0)

#if 0
#define BINT_P_UPGSC_CASES \
	case BCHP_SCIRQ0_SCIRQEN:
#else
#define BINT_P_UPGSC_CASES case 0:
#endif

#define BINT_P_TIMER_STATUS		0x00
#define BINT_P_TIMER_MASK		0x04

#define BINT_P_TIMER_CASES \
	case BCHP_TIMER_TIMER_IS:

#define BINT_P_UHF_STATUS		0x04
#define BINT_P_UHF_MASK		    0x00

#define BINT_P_UHF_CASES \
	case BCHP_UHFR_1_IMSK: \
	case BCHP_UHFR_2_IMSK:

#define BINT_P_ARB_TOP_STATUS	0x00
#define BINT_P_ARB_TOP_MASK		0x20 /* this value is subtracted below instead of the usual addition */

#if 0
#define BINT_P_ARB_TOP_CASES \
	case BCHP_ARB_TOP_CONFIG_REGS_ARCH_STS:
#else
#define BINT_P_ARB_TOP_CASES case 1:
#endif

#define BINT_P_STAT_TIMER_TICKS_PER_USEC 27

static void BINT_P_7640_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7640_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7640_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7640_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7640_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7640_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

static const BINT_P_IntMap bint_7640A0[] =
{
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX0,                 0,                   "RAV_CX0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX1,                 0,                   "RAV_CX1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX2,                 0,                   "RAV_CX2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX3,                 0,                   "RAV_CX3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX4,                 0,                   "RAV_CX4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX5,                 0,                   "RAV_CX5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX6,                 0,                   "RAV_CX6"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX7,                 0,                   "RAV_CX7"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX8,                 0,                   "RAV_CX8"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX9,                 0,                   "RAV_CX9"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX10,                0,                   "RAV_CX10"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX11,                0,                   "RAV_CX11"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX12,                0,                   "RAV_CX12"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX13,                0,                   "RAV_CX13"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX14,                0,                   "RAV_CX14"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX15,                0,                   "RAV_CX15"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX16,                0,                   "RAV_CX16"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX17,                0,                   "RAV_CX17"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX18,                0,                   "RAV_CX18"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX19,                0,                   "RAV_CX19"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX20,                0,                   "RAV_CX20"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX21,                0,                   "RAV_CX21"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX22,                0,                   "RAV_CX22"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX23,                0,                   "RAV_CX23"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVD0_CPU_INTR_0_SHIFT,        		BCHP_AVD_INTR2_0_CPU_STATUS,           0,                   "AVD_0"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_3_SHIFT,        		BCHP_BVNF_INTR2_5_R5F_STATUS,          0,                   "BVNF_3"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_SYS_CPU_INTR_SHIFT,				BCHP_SUN_L2_CPU_STATUS,                0xF000003F,          "SYS"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_MAIN_CPU_INTR_SHIFT,           		BCHP_IRQ0_IRQEN,                       ~(BCHP_IRQ0_IRQEN_gio_irqen_MASK |
                                                                                                                                 BCHP_IRQ0_IRQEN_irb_irqen_MASK),  "UPG"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_BSC_CPU_INTR_SHIFT,       		BCHP_IRQ0_IRQEN,                       ~(BCHP_IRQ0_IRQEN_iica_irqen_MASK |
                                                                                                                                 BCHP_IRQ0_IRQEN_iicb_irqen_MASK |
                                                                                                                                 BCHP_IRQ0_IRQEN_iicc_irqen_MASK),  "BSC"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_MAIN_AON_CPU_INTR_SHIFT + 32,  		BCHP_IRQ0_AON_IRQEN,                   ~(BCHP_IRQ0_AON_IRQEN_kbd1_irqen_MASK |
                                                                                                                                 BCHP_IRQ0_AON_IRQEN_gio_irqen_MASK), "UPG_AON"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SPI_CPU_INTR_SHIFT,       		BCHP_IRQ0_AON_IRQEN,                   ~(BCHP_IRQ0_AON_IRQEN_spi_irqen_MASK),  "SPI"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_UPG_BSC_AON_CPU_INTR_SHIFT + 32,         	BCHP_IRQ0_AON_IRQEN,                   ~(BCHP_IRQ0_AON_IRQEN_iicd_irqen_MASK), "BSC_AON"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_TMR_CPU_INTR_SHIFT,       		BCHP_TIMER_TIMER_IS,                   0,                   "TMR"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT,        		BCHP_BVNF_INTR2_1_R5F_STATUS,          0,                   "BVNF_1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_0_SHIFT,        		BCHP_BVNF_INTR2_0_R5F_STATUS,          0,                   "BVNF_0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_CPU_INTR_SHIFT,          		BCHP_BVNB_INTR2_CPU_STATUS,            0,                   "BVNB"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT,           		BCHP_VIDEO_ENC_INTR2_CPU_STATUS,       0,                   "VEC"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_RPTD0_CPU_INTR_SHIFT,		  	BCHP_AUD_DSP_INTH0_R5F_STATUS,         0,                   "RTPD"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RPTD1_CPU_INTR_SHIFT + 32,	  		BCHP_AUD_DSP_INTH0_1_R5F_STATUS,       0,                   "RTPD_1"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_TX_CPU_INTR_SHIFT,          		BCHP_HDMI_TX_INTR2_CPU_STATUS,         0,                   "HDMI_TX"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_PM_CPU_INTR_SHIFT + 32,       		BCHP_AON_PM_L2_CPU_STATUS,             0,                   "AON_PM"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SYS_AON_CPU_INTR_SHIFT + 32,       		BCHP_AON_L2_CPU_STATUS,                0,                   "AON_L2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_RX_CPU_INTR2_SHIFT,          		BCHP_HDMI_RX_INTR2_0_CPU_STATUS,       0,                   "HDMI_RX"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DVP_HR_CPU_INTR_SHIFT,          		BCHP_DVP_HR_INTR2_CPU_STATUS,          0,                   "DVP_HR"},
#if (BCHP_CHIP==7640 && BCHP_VER == BCHP_VER_B0)
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_M2MC_INTR_DECL_SHIFT + 32,     		BCHP_WRAP_M2MC_L2_CPU_STATUS,          ~(BCHP_WRAP_M2MC_L2_CPU_STATUS_M2MC_0_INTR_MASK |
	                                                                                                                 BCHP_WRAP_M2MC_L2_CPU_STATUS_M2MC_1_INTR_MASK),         "M2MC"},
#else
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT,           		BCHP_GFX_L2_CPU_STATUS,                0xFFFFFFC0,          "GFX"},
#endif
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AIO_CPU_INTR_SHIFT,           		BCHP_AIO_INTH_R5F_STATUS,              0,                   "AIO"},

    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,             BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31,   BINT_DONT_PROCESS_L2,"XPT_MSG_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,             BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63,   BINT_DONT_PROCESS_L2,"XPT_MSG_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,             BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95,   BINT_DONT_PROCESS_L2,"XPT_MSG_2"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,             BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127,  BINT_DONT_PROCESS_L2,"XPT_MSG_3"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,            BCHP_XPT_MSG_BUF_OVFL_INTR_00_31,      BINT_DONT_PROCESS_L2,"XPT_OVFL_0"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,            BCHP_XPT_MSG_BUF_OVFL_INTR_32_63,      BINT_DONT_PROCESS_L2,"XPT_OVFL_1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,            BCHP_XPT_MSG_BUF_OVFL_INTR_64_95,      BINT_DONT_PROCESS_L2,"XPT_OVFL_2"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,            BCHP_XPT_MSG_BUF_OVFL_INTR_96_127,     BINT_DONT_PROCESS_L2,"XPT_OVFL_3"},

    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,          BCHP_XPT_BUS_IF_INTR_STATUS_REG,       0,                   "XPT"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,          BCHP_XPT_BUS_IF_INTR_STATUS2_REG,      0,                   "XPT2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS4_REG,      0,                   "XPT4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS5_REG,      0,                   "XPT5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB0_INTR,                     0,                   "XPT_PB0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB1_INTR,                     0,                   "XPT_PB1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB2_INTR,                     0,                   "XPT_PB2"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB3_INTR,                     0,                   "XPT_PB3"},

        /* WARNING:   HIF_CPU_INTR interrupts are shared with Linux Kernel, make sure InvalidMask matches BCM Driver interrupt_table.c */
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT,             BCHP_HIF_INTR2_CPU_STATUS, ~(BCHP_HIF_INTR2_CPU_STATUS_SHARF_MEM_DMA0_DONE_MASK |
                                                                                                         BCHP_HIF_INTR2_CPU_STATUS_SHARF_FAIL0_INTR_MASK |
                                                                                                         BCHP_HIF_INTR2_CPU_STATUS_SHARF_FAIL1_INTR_MASK |
                                                                                                         BCHP_HIF_INTR2_CPU_STATUS_SHARF_ERR_INTR_MASK),    "HIF"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BSP_CPU_INTR_SHIFT,           		BCHP_BSP_CONTROL_INTR2_CPU_STATUS,   0,                   "BSP"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_FE_CPU_INTR_SHIFT + 32,   		BCHP_XPT_FE_INTR_STATUS0_REG,           0,                   "XPT_FE0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_FE_CPU_INTR_SHIFT + 32,   		BCHP_XPT_FE_INTR_STATUS1_REG,           0,                   "XPT_FE1"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_PCROFFSET_INTERRUPT0_STATUS,  0,                   "XPT_PCROFF0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_PCROFFSET_INTERRUPT1_STATUS,  0,                   "XPT_PCROFF1"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_FGX_CPU_INTR_SHIFT + 32,			BCHP_FGX_TO_HIF_INTR2_CPU_STATUS,      0,  		    "FGX"},

	{ -1, 0, 0, NULL}
};

static const BINT_Settings bint_7640Settings =
{
	BINT_P_7640_SetInt,
	BINT_P_7640_ClearInt,
	BINT_P_7640_SetMask,
	BINT_P_7640_ClearMask,
	BINT_P_7640_ReadMask,
	BINT_P_7640_ReadStatus,
	bint_7640A0,
	"7640"
};

static void BINT_P_7640_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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

static void BINT_P_7640_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	BDBG_MSG(("ClearInt %#x:%d", baseAddr, shift));
	switch( baseAddr )
	{
		BINT_P_STANDARD_L2_CASES
			BREG_Write32( regHandle, baseAddr + BINT_P_STD_CLEAR, 1ul<<shift);
			break;
		BINT_P_XPT_STATUS_CASES
		BINT_P_XPT_FE_STATUS_CASES
			BREG_Write32( regHandle, baseAddr + BINT_P_XPT_STATUS, ~(1ul<<shift));
			break;
		BINT_P_XPT_RAVE_CASES
			BREG_Write32( regHandle, baseAddr + BINT_P_RAVE_STATUS, (1ul<<shift));
			break;

		BINT_P_TIMER_CASES
			BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_STATUS, 1ul<<shift);
			break;
		BINT_P_UPGSC_CASES
		BINT_P_IRQ0_CASES
			/* Has to be cleared at the source */
			break;

		BINT_P_ARB_TOP_CASES
			/* Write 1 to clear int bit. */
			BREG_Write32( regHandle, baseAddr + BINT_P_ARB_TOP_STATUS, ~( 1ul << shift ) );
			break;
		default:
			/* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
			break;
	}
}

static void BINT_P_7640_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
        BINT_P_XPT_FE_STATUS_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_FE_ENABLE);
		intEnable &= ~(1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_XPT_ENABLE, intEnable);
		break;
	BINT_P_XPT_RAVE_CASES
		RaveEnReg = GetRaveIntEnableOffset( baseAddr );
		intEnable = BREG_Read32( regHandle, RaveEnReg );
		intEnable &= ~(1ul<<shift);
		BREG_Write32( regHandle, RaveEnReg, intEnable);
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
		intEnable |= (BCHP_IRQ0_IRQSTAT_uairq_MASK|BCHP_IRQ0_IRQSTAT_ubirq_MASK);
		BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable);
		break;
	BINT_P_UPGSC_CASES
		/* TODO: The linux kernel may also use this interrupt register, so we must disable interrupts */
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
		intEnable &= ~(1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE, intEnable );
		break;

	BINT_P_ARB_TOP_CASES
		intEnable = BREG_Read32( regHandle, baseAddr - BINT_P_ARB_TOP_MASK );
		intEnable |= ( 1ul << shift );
		BREG_Write32( regHandle, baseAddr - BINT_P_ARB_TOP_MASK, intEnable);
		break;

	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}
}

static void BINT_P_7640_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
	BINT_P_XPT_FE_STATUS_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_FE_ENABLE);
		intEnable |= 1ul<<shift;
		BREG_Write32( regHandle, baseAddr + BINT_P_XPT_ENABLE, intEnable);
		break;
	BINT_P_XPT_RAVE_CASES
		RaveEnReg = GetRaveIntEnableOffset( baseAddr );
		intEnable = BREG_Read32( regHandle, RaveEnReg );
		intEnable |= (1ul<<shift);
		BREG_Write32( regHandle, RaveEnReg, intEnable);
		break;

	BINT_P_IRQ0_CASES
		/* we need to keep uart a/b interrupts enabled */
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
		intEnable |= (1ul<<shift);
		intEnable |= (BCHP_IRQ0_IRQSTAT_uairq_MASK|BCHP_IRQ0_IRQSTAT_ubirq_MASK);
		BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable );
		break;
	BINT_P_UPGSC_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we should never be touching this bit in the L2 interrupt manager.
		*/
		BDBG_ASSERT( shift != BCHP_IRQ0_IRQSTAT_uairq_SHIFT && shift != BCHP_IRQ0_IRQSTAT_ubirq_SHIFT );

		/* TODO: The linux kernel may also use this interrupt register, so we must disable interrupts */
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
		intEnable |= 1ul<<shift;
		BREG_Write32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE, intEnable );
		break;
	BINT_P_TIMER_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK );
		intEnable |= (1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_MASK, intEnable );
		break;

	BINT_P_ARB_TOP_CASES
		intEnable = BREG_Read32( regHandle, baseAddr - BINT_P_ARB_TOP_MASK );
		intEnable &= ~( 1ul << shift );
		BREG_Write32( regHandle, baseAddr - BINT_P_ARB_TOP_MASK, intEnable);
		break;

	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}
}

static uint32_t BINT_P_7640_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
{
	uint32_t RaveEnReg = 0;

	BDBG_MSG(("ReadMask %#x", baseAddr));
	switch( baseAddr )
	{
	BINT_P_STANDARD_L2_CASES
		return BREG_Read32(regHandle, baseAddr + BINT_P_STD_MASK_STATUS);
	BINT_P_XPT_STATUS_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_ENABLE );
	BINT_P_XPT_FE_STATUS_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_FE_ENABLE );
	BINT_P_XPT_RAVE_CASES
		RaveEnReg = GetRaveIntEnableOffset( baseAddr );
		return ~BREG_Read32( regHandle, RaveEnReg );

	BINT_P_TIMER_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK );
	BINT_P_IRQ0_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE );
	BINT_P_UPGSC_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uXrta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager
		*/
		return ~(BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE )) | BCHP_IRQ0_IRQSTAT_uairq_MASK | BCHP_IRQ0_IRQSTAT_ubirq_MASK;

	BINT_P_ARB_TOP_CASES
		return BREG_Read32( regHandle, baseAddr - BINT_P_ARB_TOP_MASK );

		default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

static uint32_t BINT_P_7640_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
	BDBG_MSG(("ReadStatus %#x", baseAddr));
	switch( baseAddr )
	{
	BINT_P_STANDARD_L2_CASES
		return BREG_Read32(regHandle, baseAddr + BINT_P_STD_STATUS);
	BINT_P_XPT_STATUS_CASES
	BINT_P_XPT_FE_STATUS_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_STATUS );
	BINT_P_XPT_RAVE_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_RAVE_STATUS );

	BINT_P_TIMER_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_STATUS );
	BINT_P_IRQ0_CASES
		/* we need to keep uart a/b interrupts enabled, and mask them for application */
		return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS ) & ~(BCHP_IRQ0_IRQSTAT_uairq_MASK|BCHP_IRQ0_IRQSTAT_ubirq_MASK);
		break;
	BINT_P_UPGSC_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager
		*/
		return BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE ) & ~(BCHP_IRQ0_IRQSTAT_uairq_MASK|BCHP_IRQ0_IRQSTAT_ubirq_MASK);

	BINT_P_ARB_TOP_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_ARB_TOP_STATUS );
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

const BINT_Settings *BINT_7640_GetSettings( void )
{
	return &bint_7640Settings;
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

