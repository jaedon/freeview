/***************************************************************************
 *	   Copyright (c) 2003-2008, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7440.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 10/24/08 3:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7440/bint_7440.c $
 * 
 * Hydra_Software_Devel/12   10/24/08 3:52p dlwin
 * PR 45928: Corrected a problem DSP1 interrupt, used the incorrect
 * interrupt register.
 * 
 * Hydra_Software_Devel/11   10/9/08 1:15p dlwin
 * PR 45928: Add support for DSP1 interrupts to Host
 * 
 * Hydra_Software_Devel/10   9/27/07 4:58p tdo
 * PR35401: 7440C0: Bring-up of Application Framework.
 * 
 * Hydra_Software_Devel/9   4/23/07 3:14p nilesh
 * PR22766: Unmasked FGT interrupts
 * 
 * Hydra_Software_Devel/8   4/18/07 11:06a albertl
 * PR29635:  Removed comments.
 * 
 * Hydra_Software_Devel/7   4/18/07 11:04a albertl
 * PR29635:  Added support for fgx_to_hif.
 * 
 * Hydra_Software_Devel/6   4/10/07 12:20a vijayk
 * PR 29635: Add interrupt support for SHARF
 * 
 * Hydra_Software_Devel/5   4/9/07 11:11p vijayk
 * PR 29635: Add interrupt support for SHARF module
 * 
 * Hydra_Software_Devel/4   11/27/06 7:06p albertl
 * PR23111:  Added BCHP_ARB_TOP interrupts.
 * 
 * Hydra_Software_Devel/3   10/16/06 1:52p nilesh
 * PR23113: Added support for AVD 1 Core interrupts
 * 
 * Hydra_Software_Devel/2   8/17/06 6:19p katrep
 * PR23114: Added support for 7440 chip
 * 
 * Hydra_Software_Devel/1   7/12/06 3:36p albertl
 * PR22276:  Initial revision for 7440.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bint_7440.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"

/* Standard L2 stuff */
#include "bchp_aio_inth.h"
#include "bchp_arb_top_config_regs.h"
#include "bchp_aud_dsp_inth0.h"
#include "bchp_aud_dsp_inth0_1.h"
#include "bchp_avd_intr2_0.h"
#include "bchp_avd_intr2_1.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_gfx_l2.h"
#include "bchp_hdmi_intr2.h"
#include "bchp_hif_intr2.h"
#include "bchp_pci_bridge_intr2.h"
#include "bchp_sun_l2.h"
#include "bchp_usb_intr2.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_bsp_control_intr2.h"

#if (BCHP_VER > BCHP_VER_A0)
#include "bchp_fgx_to_hif_intr2.h"
#endif

/* XPT */
#include "bchp_xpt_fe.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"
#include "bchp_xpt_pb2.h"
#include "bchp_xpt_rave.h"
#include "bchp_xpt_msg.h"
#include "bchp_xpt_pcroffset0.h"
#include "bchp_xpt_pcroffset1.h"

/* UARTs, keypad, I2C */
#include "bchp_irq0.h"

/* Smartcard interrupts. */
#include "bchp_scirq0.h"

/* Timer */
#include "bchp_timer.h"

BDBG_MODULE(interruptinterface_7440);

#define BINT_P_STD_STATUS		0x00
#define BINT_P_STD_SET			0x04
#define BINT_P_STD_CLEAR		0x08
#define BINT_P_STD_MASK_STATUS	0x0C
#define BINT_P_STD_MASK_SET		0x10
#define BINT_P_STD_MASK_CLEAR	0x14


#define BINT_P_STANDARD_L2_CASES_BASE \
	case BCHP_AIO_INTH_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_1_R5F_STATUS: \
	case BCHP_AVD_INTR2_0_CPU_STATUS: \
	case BCHP_AVD_INTR2_1_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_3_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_4_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_HDMI_INTR2_CPU_STATUS: \
	case BCHP_HIF_INTR2_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BSP_CONTROL_INTR2_CPU_STATUS: \
	case BCHP_SUN_L2_CPU_STATUS:



#if (BCHP_VER == BCHP_VER_A0)
#define BINT_P_STANDARD_L2_CASES \
	BINT_P_STANDARD_L2_CASES_BASE
#elif ((BCHP_VER == BCHP_VER_B0) || (BCHP_VER == BCHP_VER_C0))
#define BINT_P_STANDARD_L2_CASES \
	BINT_P_STANDARD_L2_CASES_BASE \
	case BCHP_FGX_TO_HIF_INTR2_CPU_STATUS:
#endif /* BCHP_VER == BCHP_VER_A0 */

#define	BINT_P_IRQ0_CASES \
	case BCHP_IRQ0_IRQEN: 

#define BINT_P_IRQ0_ENABLE		0
#define BINT_P_IRQ0_STATUS		4


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
    case BCHP_XPT_DPCR0_INTR_STATUS_REG: \
	case BCHP_XPT_DPCR1_INTR_STATUS_REG: \
	case BCHP_XPT_PCROFFSET0_INTERRUPT_STATUS: \
	case BCHP_XPT_PCROFFSET1_INTERRUPT_STATUS: \
    case BCHP_XPT_FE_INTR_STATUS_REG: 

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

#define BINT_P_UHF_STATUS		0x04
#define BINT_P_UHF_MASK		    0x00
	
#define BINT_P_UHF_CASES \
	case BCHP_UHFR_1_IMSK: \
	case BCHP_UHFR_2_IMSK:

#define BINT_P_ARB_TOP_STATUS	0x00
#define BINT_P_ARB_TOP_MASK		0x20 /* this value is subtracted below instead of the usual addition */

#define BINT_P_ARB_TOP_CASES \
	case BCHP_ARB_TOP_CONFIG_REGS_ARCH_STS:

#define BINT_P_STAT_TIMER_TICKS_PER_USEC 27

static void BINT_P_7440_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7440_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7440_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7440_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7440_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7440_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

static const BINT_P_IntMap bint_7440A0[] =
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

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVD0_CPU_INTR_0_SHIFT,        		BCHP_AVD_INTR2_0_CPU_STATUS,      	   0,                   "AVD_0"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_3_SHIFT,        		BCHP_BVNF_INTR2_5_R5F_STATUS,          0,                   "BVNF_3"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SC_CPU_INTR_SHIFT,        		BCHP_SCIRQ0_SCIRQEN,                   0,                   "SC"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_SUN_CPU_INTR_SHIFT,					BCHP_SUN_L2_CPU_STATUS,                0xF000003F,          "SUN"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT,           		BCHP_IRQ0_IRQEN,                       0xFFFFFC00,          "UPG"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_BSC_CPU_INTR_SHIFT,       		BCHP_IRQ0_IRQEN,                       0xE0FFFFFF,          "BSC"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SPI_CPU_INTR_SHIFT,       		BCHP_IRQ0_IRQEN,                       0xFFEFFFFF,          "SPI"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_TMR_CPU_INTR_SHIFT,       		BCHP_TIMER_TIMER_IS,                   0,                   "TMR"},
	
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT,        		BCHP_BVNF_INTR2_1_R5F_STATUS,          0,                   "BVNF_1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_0_SHIFT,        		BCHP_BVNF_INTR2_0_R5F_STATUS,          0,                   "BVNF_0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_CPU_INTR_SHIFT,          		BCHP_BVNB_INTR2_CPU_STATUS,            0,                   "BVNB"},
																	
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT,           		BCHP_VIDEO_ENC_INTR2_CPU_STATUS,       0,                   "VEC"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_RPTD_CPU_INTR_SHIFT,		  		BCHP_AUD_DSP_INTH0_R5F_STATUS,         0,                   "RTPD"},
	
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RPTD1_CPU_INTR_SHIFT + 32,	  		BCHP_AUD_DSP_INTH0_1_R5F_STATUS,       0,                   "RTPD_1"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_CPU_INTR_SHIFT,          		BCHP_HDMI_INTR2_CPU_STATUS,            0,                   "HDMI"},
	
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT,           		BCHP_GFX_L2_CPU_STATUS,                0xFFFFFFC0,          "GFX"},
	
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AIO_CPU_INTR_SHIFT,           		BCHP_AIO_INTH_R5F_STATUS,              0,                   "AIO"},
	
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,       		BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31,   BINT_DONT_PROCESS_L2,"XPT_MSG_0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,       		BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63,   BINT_DONT_PROCESS_L2,"XPT_MSG_1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,       		BCHP_XPT_MSG_BUF_DAT_RDY_INTR_64_95,   BINT_DONT_PROCESS_L2,"XPT_MSG_2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,       		BCHP_XPT_MSG_BUF_DAT_RDY_INTR_96_127,  BINT_DONT_PROCESS_L2,"XPT_MSG_3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,      		BCHP_XPT_MSG_BUF_OVFL_INTR_00_31,      BINT_DONT_PROCESS_L2,"XPT_OVFL_0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,      		BCHP_XPT_MSG_BUF_OVFL_INTR_32_63,      BINT_DONT_PROCESS_L2,"XPT_OVFL_1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,      		BCHP_XPT_MSG_BUF_OVFL_INTR_64_95,      BINT_DONT_PROCESS_L2,"XPT_OVFL_2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,      		BCHP_XPT_MSG_BUF_OVFL_INTR_96_127,     BINT_DONT_PROCESS_L2,"XPT_OVFL_3"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS_REG,       0,                   "XPT"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS2_REG,      0,                   "XPT2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS4_REG,      0,                   "XPT4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS5_REG,      0,                   "XPT5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB0_INTR,                     0,                   "XPT_PB0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB1_INTR,                     0,                   "XPT_PB1"},
    { BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB2_INTR,                     0,                   "XPT_PB2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_ICAM_CPU_INTR_SHIFT,   			BCHP_XPT_BUS_IF_INTR_STATUS3_REG,      0,                   "XPT_ICAM"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT,           		BCHP_HIF_INTR2_CPU_STATUS,             0xfffff000,          "HIF"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BSP_CPU_INTR_SHIFT,           		BCHP_BSP_CONTROL_INTR2_CPU_STATUS,   0,                   "BSP"},
	
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_FE_CPU_INTR_SHIFT + 32,   		BCHP_XPT_FE_INTR_STATUS_REG,           0,                   "XPT_FE"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_DPCR0_INTR_STATUS_REG,        0,                   "XPT_DPCR0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_DPCR1_INTR_STATUS_REG,        0,                   "XPT_DPCR1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_PCROFFSET0_INTERRUPT_STATUS,  0,                   "XPT_PCROFF0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_PCROFFSET1_INTERRUPT_STATUS,  0,                   "XPT_PCROFF1"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_MSG_STAT_CPU_INTR_SHIFT + 32,  	BCHP_XPT_MSG_DAT_ERR_INTR,   		   0,                   "XPT_MSG_ERR"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_AVD1_CPU_INTR_1_SHIFT + 32,         BCHP_AVD_INTR2_1_CPU_STATUS,          0,                   "AVD_1"},


#if (BCHP_VER > BCHP_VER_A0)
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_FGX_CPU_INTR_SHIFT + 32,	BCHP_FGX_TO_HIF_INTR2_CPU_STATUS,      0,                   "FGX"},
#endif
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_MEMARB_CPU_INTR_SHIFT + 32,         BCHP_ARB_TOP_CONFIG_REGS_ARCH_STS,     0xFFFFFFF0,                   "MEMARB"},

	{ -1, 0, 0, NULL}
};

static const BINT_Settings bint_7440Settings =
{
	BINT_P_7440_SetInt,
	BINT_P_7440_ClearInt,
	BINT_P_7440_SetMask,
	BINT_P_7440_ClearMask,
	BINT_P_7440_ReadMask,
	BINT_P_7440_ReadStatus,	
	bint_7440A0,
	"7440"
};

static void BINT_P_7440_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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

static void BINT_P_7440_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
			/* Has to cleared at the source */
			break;
		BINT_P_XPT_MSG_ERR_CASES
			/* Write 0 to clear the int bit. Writing 1's are ingored. */
			BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS, ~( 1ul << shift ) );
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

static void BINT_P_7440_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
		intEnable |= (BCHP_IRQ0_IRQSTAT_uairq_MASK|BCHP_IRQ0_IRQSTAT_ubirq_MASK);
		BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable);
		break;
	BINT_P_UPGSC_CASES
		/* TODO: The linux kernel may also use this interrupt register, so we must disable interrupts */
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
		intEnable &= ~(1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE, intEnable );
		break;

	BINT_P_XPT_MSG_ERR_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
		intEnable &= ~( 1ul << shift );
		BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE, intEnable);
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

static void BINT_P_7440_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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

	BINT_P_XPT_MSG_ERR_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
		intEnable |= ( 1ul << shift );
		BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE, intEnable);
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

static uint32_t BINT_P_7440_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
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
	BINT_P_UPGSC_CASES
		/* 
			The BCHP_IRQ0_IRQSTAT_uXrta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager 
		*/
		return ~(BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE )) | BCHP_IRQ0_IRQSTAT_uairq_MASK | BCHP_IRQ0_IRQSTAT_ubirq_MASK;

	BINT_P_XPT_MSG_ERR_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
		
	BINT_P_ARB_TOP_CASES
		return BREG_Read32( regHandle, baseAddr - BINT_P_ARB_TOP_MASK );

		default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

static uint32_t BINT_P_7440_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
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
		return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS ) & ~(BCHP_IRQ0_IRQSTAT_uairq_MASK|BCHP_IRQ0_IRQSTAT_ubirq_MASK);
		break;
	BINT_P_UPGSC_CASES
		/* 
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager 
		*/
		return BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE ) & ~(BCHP_IRQ0_IRQSTAT_uairq_MASK|BCHP_IRQ0_IRQSTAT_ubirq_MASK);

	BINT_P_XPT_MSG_ERR_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS );

	BINT_P_ARB_TOP_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_ARB_TOP_STATUS );
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

const BINT_Settings *BINT_7440_GetSettings( void )
{
	return &bint_7440Settings;
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

