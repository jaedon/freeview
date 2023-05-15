/***************************************************************************
 *	   Copyright (c) 2003-2007, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7401.c $
 * $brcm_Revision: Hydra_Software_Devel/27 $
 * $brcm_Date: 2/15/07 12:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7401/bint_7401.c $
 * 
 * Hydra_Software_Devel/27   2/15/07 12:01p erickson
 * PR26657: optimized BINT_Isr. added BINT_IS_STANDARD to allow standard
 * interrupts to be processed inside bint.c.
 * 
 * Hydra_Software_Devel/26   11/7/06 3:14p erickson
 * PR25108: add basemodule support for 7403
 * 
 * Hydra_Software_Devel/25   10/31/06 3:21p erickson
 * PR25108: add 7403 support
 * 
 * Hydra_Software_Devel/24   10/31/06 8:10a erickson
 * PR25108: add 7403 support
 * 
 * Hydra_Software_Devel/23   8/15/06 11:55a erickson
 * PR22726: allow UART A and B to be used if compile time flag is set.
 * remove invalid code for smartcard. added comments.
 * 
 * Hydra_Software_Devel/22   6/5/06 4:28p erickson
 * PR21941: cleaned up minor warnings
 * 
 * Hydra_Software_Devel/21   5/26/06 10:32a erickson
 * PR21392: removed old callbacks
 * 
 * Hydra_Software_Devel/20   5/9/06 11:27a gmullen
 * PR20632: DPCR,PCROffset ints moved to STATUS case, added PB2 and 3 for
 * B0
 * 
 * Hydra_Software_Devel/19   4/19/06 5:19p erickson
 * PR21116: mask soft modem bits from BINT handling
 * 
 * Hydra_Software_Devel/18   4/7/06 3:09p katrep
 * PR20415: Moved the DPCR0,DPCR1,PCROFFSET interrupts to XPT_STATUS_CASES
 * 
 * Hydra_Software_Devel/17   4/6/06 11:23a gmullen
 * PR20632: Support for 7401 B0
 * 
 * Hydra_Software_Devel/16   2/15/06 5:30p vsilyaev
 * PR 19693: Added support for acquiring interrupt rate
 * 
 * Hydra_Software_Devel/15   12/5/05 2:03p gmullen
 * PR15309: Added XPT_MSG_DAT_ERR_INTR support
 * 
 * Hydra_Software_Devel/14   11/11/05 11:27a qcheng
 * PR18088: add interrupt support for ICAM and BSP in BCM97401 Linux
 * 
 * Hydra_Software_Devel/13   11/9/05 3:48p erickson
 * PR17936: remove incorrect ~ in BINT_P_7401_ClearMask
 * 
 * Hydra_Software_Devel/12   11/1/05 6:08p brianlee
 * PR14344: Fixed UPG irq mask to handle UART C interrupt.
 * 
 * Hydra_Software_Devel/11   10/28/05 2:41p jkim
 * PR14344: Add SC interrupt
 * 
 * Hydra_Software_Devel/10   10/20/05 11:44a vsilyaev
 * PR16923: Fixed C++ comments
 * 
 * Hydra_Software_Devel/9   10/11/05 2:56p jkim
 * PR16923: modify UHF interrupt according to new RDB files
 * 
 * Hydra_Software_Devel/8   9/19/05 11:38a gmullen
 * PR15309: Added support for AUD_DSP_INTH0_R5F_STATUS and
 * AIO_INTH_R5F_STATUS .
 * 
 * Hydra_Software_Devel/7   9/1/05 2:52p jasonh
 * PR 16827: Removed MC_CPU_INTR since it wasn't enabled in driver.
 * 
 * Hydra_Software_Devel/6   8/1/05 6:01p vsilyaev
 * PR 15309: 7401 uses IRQ0_ENABLE register to map UPG interrupts.
 * 
 * Hydra_Software_Devel/5   7/13/05 3:36p vsilyaev
 * PR 15309: Added workaround for UARTB.
 * 
 * Hydra_Software_Devel/4   7/13/05 1:58p vsilyaev
 * PR 15309: Fixed missing cases for SC and XPT interrupts
 * 
 * Hydra_Software_Devel/3   7/6/05 6:54p gmullen
 * PR15309: Added interrupt map for secure processor
 * 
 * Hydra_Software_Devel/2   6/27/05 5:10p gmullen
 * PR15309: Fixed bug in interrupt map, added ReadTimer and GetElapsedTime
 * private functions.
 * 
 * Hydra_Software_Devel/1   6/27/05 3:20p gmullen
 * PR15309: Added support for XPT interrupts.
 * 
 ***************************************************************************/
#include "bstd.h"
#if BCHP_CHIP == 7403
#include "bint_7403.h"
#elif BCHP_CHIP == 7401
#include "bint_7401.h"
#else
#error BCHP_CHIP not supported
#endif

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"

/* Standard L2 stuff */
#include "bchp_aio_inth.h"
#include "bchp_aud_dsp_inth0.h"
#include "bchp_avd0_intr2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_2.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_gfx_l2.h"
#include "bchp_hdmi_intr2.h"
#include "bchp_hif_intr2.h"
#include "bchp_mcif_intr2.h"
#include "bchp_pci_bridge_intr2.h"
#include "bchp_rfm_l2.h"
#include "bchp_sun_l2.h"
#include "bchp_uhfr_1.h"
#include "bchp_usb_intr2.h"
#include "bchp_video_enc_intr2.h"

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
#include "bchp_bsp_control_intr2.h"
#else
#include "bchp_bsp_nonsecure_intr2.h"
#endif


/* XPT */
#include "bchp_xpt_fe.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_pb1.h"
#include "bchp_xpt_rave.h"
#include "bchp_xpt_msg.h"

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
#include "bchp_xpt_pcroffset0.h"
#include "bchp_xpt_pb2.h"
#include "bchp_xpt_pb3.h"
#else
#include "bchp_xpt_pcroffset.h"
#endif

/* UARTs, keypad, I2C */
#include "bchp_irq0.h"

/* Smartcard interrupts. */
#include "bchp_scirq0.h"

/* Timer */
#include "bchp_timer.h"

BDBG_MODULE(interruptinterface_7401);

#define BINT_P_STD_STATUS		0x00
#define BINT_P_STD_SET			0x04
#define BINT_P_STD_CLEAR		0x08
#define BINT_P_STD_MASK_STATUS	0x0C
#define BINT_P_STD_MASK_SET		0x10
#define BINT_P_STD_MASK_CLEAR	0x14


#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
#define BINT_P_STANDARD_L2_CASES \
	case BCHP_AIO_INTH_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_R5F_STATUS: \
	case BCHP_AVD0_INTR2_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_2_R5F_STATUS: \
	case BCHP_BVNF_INTR2_3_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_4_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_HDMI_INTR2_CPU_STATUS: \
	case BCHP_HIF_INTR2_CPU_STATUS: \
	case BCHP_MCIF_INTR2_CPU_STATUS: \
	case BCHP_RFM_L2_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BSP_CONTROL_INTR2_CPU_STATUS: 
#else
#define BINT_P_STANDARD_L2_CASES \
	case BCHP_AIO_INTH_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_R5F_STATUS: \
	case BCHP_AVD0_INTR2_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_2_R5F_STATUS: \
	case BCHP_BVNF_INTR2_3_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_4_AVD0_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_HDMI_INTR2_CPU_STATUS: \
	case BCHP_HIF_INTR2_CPU_STATUS: \
	case BCHP_MCIF_INTR2_CPU_STATUS: \
	case BCHP_RFM_L2_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BSP_NONSECURE_INTR2_CPU_STATUS: 
#endif

#define BINT_P_IRQ0_ENABLE		0
#define BINT_P_IRQ0_STATUS		4

#define	BINT_P_IRQ0_CASES \
	case BCHP_IRQ0_IRQEN: 

#define BINT_P_XPT_STATUS			0x00
#define BINT_P_XPT_ENABLE			0x04

/* There is no constant address mapping from RAVE status to RAVE enable registers. */
#define BINT_P_RAVE_STATUS			0x00

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)

#define BINT_P_XPT_STATUS_CASES \
	case BCHP_XPT_BUS_IF_INTR_STATUS_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS2_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS3_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS4_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS5_REG: \
	case BCHP_XPT_PB0_INTR: \
	case BCHP_XPT_PB1_INTR:  \
	case BCHP_XPT_PB2_INTR:  \
	case BCHP_XPT_PB3_INTR:  \
	case BCHP_XPT_DPCR0_INTR_STATUS_REG: \
	case BCHP_XPT_DPCR1_INTR_STATUS_REG: \
	case BCHP_XPT_PCROFFSET0_INTERRUPT_STATUS: \
	case BCHP_XPT_FE_INTR_STATUS_REG:

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

#else

#define BINT_P_XPT_STATUS_CASES \
	case BCHP_XPT_BUS_IF_INTR_STATUS_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS2_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS3_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS4_REG: \
	case BCHP_XPT_BUS_IF_INTR_STATUS5_REG: \
	case BCHP_XPT_PB0_INTR: \
	case BCHP_XPT_PB1_INTR:  \
	case BCHP_XPT_DPCR0_INTR_STATUS_REG: \
	case BCHP_XPT_DPCR1_INTR_STATUS_REG: \
	case BCHP_XPT_PCROFFSET_INTERRUPT_STATUS: \
	case BCHP_XPT_FE_INTR_STATUS_REG:

#define BINT_P_XPT_RAVE_CASES \
    case BCHP_XPT_RAVE_INT_CX0: \
    case BCHP_XPT_RAVE_INT_CX1: \
    case BCHP_XPT_RAVE_INT_CX2: \
    case BCHP_XPT_RAVE_INT_CX3: \
    case BCHP_XPT_RAVE_INT_CX4: \
    case BCHP_XPT_RAVE_INT_CX5: \
    case BCHP_XPT_RAVE_INT_RC0: \
    case BCHP_XPT_RAVE_INT_RC1: \
    case BCHP_XPT_RAVE_INT_RC2: \
    case BCHP_XPT_RAVE_INT_RC3: \
    case BCHP_XPT_RAVE_INT_RC4: \
    case BCHP_XPT_RAVE_INT_RC5: \
    case BCHP_XPT_RAVE_INT_RC6: \
    case BCHP_XPT_RAVE_INT_RC7:

#endif

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
#define BINT_P_UPGSC_STATUS (0)

#define BINT_P_UPGSC_CASES \
	case BCHP_SCIRQ0_SCIRQEN:

#define BINT_P_TIMER_STATUS		0x00
#define BINT_P_TIMER_MASK		0x04

#define BINT_P_TIMER_CASES \
	case BCHP_TIMER_TIMER_IS:

#define BINT_P_UHF_STATUS		0x04
#define BINT_P_UHF_MASK		    0x00
	
#define BINT_P_UHF_CASES \
	case BCHP_UHFR_1_IMSK:

#define BINT_P_STAT_TIMER_TICKS_PER_USEC 27

static void BINT_P_7401_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7401_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7401_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7401_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7401_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7401_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

#if 0
/* unused static functions */
static uint32_t BINT_P_7401_ReadTimer( BREG_Handle regHandle );
static uint32_t BINT_P_7401_GetElapsedTime( BREG_Handle regHandle, uint32_t ulTimerStart );
#endif

static const BINT_P_IntMap bint_7401A0[] =
{
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX0,                 0,                   "RAV_CX0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX1,                 0,                   "RAV_CX1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX2,                 0,                   "RAV_CX2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX3,                 0,                   "RAV_CX3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX4,                 0,                   "RAV_CX4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_CX5,                 0,                   "RAV_CX5"},
#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
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
#else
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC0,                 0,                   "RAV_RC0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC1,                 0,                   "RAV_RC1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC2,                 0,                   "RAV_RC2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC3,                 0,                   "RAV_RC3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC4,                 0,                   "RAV_RC4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC5,                 0,                   "RAV_RC5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC6,                 0,                   "RAV_RC6"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_RAV_CPU_INTR_SHIFT,       		BCHP_XPT_RAVE_INT_RC7,                 0,                   "RAV_RC7"},
#endif

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AVD0_CPU_INTR_0_SHIFT|BINT_IS_STANDARD, BCHP_AVD0_INTR2_CPU_STATUS,            0,                   "AVD_0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_3_SHIFT|BINT_IS_STANDARD, BCHP_BVNF_INTR2_5_R5F_STATUS,          0,                   "BVNF_3"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UHF1_CPU_INTR_SHIFT,          		BCHP_UHFR_1_IMSK,                      0,                   "UHFR"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SC_CPU_INTR_SHIFT,        		BCHP_SCIRQ0_SCIRQEN,                 0,                  	"SC"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT,           		BCHP_IRQ0_IRQEN,                       0xFFFFFC00,          "UPG"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_BSC_CPU_INTR_SHIFT,       		BCHP_IRQ0_IRQEN,                       0xF0FFFFFF,          "BSC"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SPI_CPU_INTR_SHIFT,       		BCHP_IRQ0_IRQEN,                       0xFFEFFFFF,          "SPI"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_TMR_CPU_INTR_SHIFT,       		BCHP_TIMER_TIMER_IS,                   0,                   "TMR"},
	
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_RFM_CPU_INTR_SHIFT|BINT_IS_STANDARD,    BCHP_RFM_L2_CPU_STATUS,                0,                   "RFM"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_2_SHIFT|BINT_IS_STANDARD, BCHP_BVNF_INTR2_2_R5F_STATUS,          0,                   "BVNF_2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT|BINT_IS_STANDARD, BCHP_BVNF_INTR2_1_R5F_STATUS,          0,                   "BVNF_1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_0_SHIFT|BINT_IS_STANDARD, BCHP_BVNF_INTR2_0_R5F_STATUS,          0,                   "BVNF_0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_CPU_INTR_SHIFT|BINT_IS_STANDARD,   BCHP_BVNB_INTR2_CPU_STATUS,            0,                   "BVNB"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT|BINT_IS_STANDARD,    BCHP_VIDEO_ENC_INTR2_CPU_STATUS,       0,                   "VEC"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_RPTD_CPU_INTR_SHIFT|BINT_IS_STANDARD,   BCHP_AUD_DSP_INTH0_R5F_STATUS,         0,                   "RPTD"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_CPU_INTR_SHIFT|BINT_IS_STANDARD,   BCHP_HDMI_INTR2_CPU_STATUS,            0,                   "HDMI"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT|BINT_IS_STANDARD,    BCHP_GFX_L2_CPU_STATUS,                0xFFFFFFE0,          "GFX"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AIO_CPU_INTR_SHIFT|BINT_IS_STANDARD,    BCHP_AIO_INTH_R5F_STATUS,              0,                   "AIO"},
	
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
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_ICAM_CPU_INTR_SHIFT,   			BCHP_XPT_BUS_IF_INTR_STATUS3_REG,      0,                   "XPT3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS4_REG,      0,                   "XPT4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_BUS_IF_INTR_STATUS5_REG,      0,                   "XPT5"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB0_INTR,                     0,                   "XPT_PB0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB1_INTR,                     0,                   "XPT_PB1"},

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB2_INTR,                     0,                   "XPT_PB2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,    		BCHP_XPT_PB3_INTR,                     0,                   "XPT_PB3"},
#endif

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT|BINT_IS_STANDARD, BCHP_HIF_INTR2_CPU_STATUS,             0,                   "HIF"},

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BSP_CPU_INTR_SHIFT|BINT_IS_STANDARD, BCHP_BSP_CONTROL_INTR2_CPU_STATUS,   0,                     "BSP"},
#else
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BSP_CPU_INTR_SHIFT|BINT_IS_STANDARD, BCHP_BSP_NONSECURE_INTR2_CPU_STATUS, 0,                     "BSP"},
#endif
	
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_FE_CPU_INTR_SHIFT + 32,   		BCHP_XPT_FE_INTR_STATUS_REG,           0,                   "XPT_FE"},

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_DPCR0_INTR_STATUS_REG,        0,                   "XPT_DPCR0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_DPCR1_INTR_STATUS_REG,        0,                   "XPT_DPCT1"},

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_PCROFFSET0_INTERRUPT_STATUS,   0,                  "XPT_PCROFF0"},
#else
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_PCR_CPU_INTR_SHIFT + 32,  		BCHP_XPT_PCROFFSET_INTERRUPT_STATUS,   0,                   "XPT_PCROFF0"},
#endif

	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_XPT_MSG_STAT_CPU_INTR_SHIFT + 32,  	BCHP_XPT_MSG_DAT_ERR_INTR,   		   0,                   "XPT_MSG_ERR"},

	{ -1, 0, 0, NULL }
};

static const BINT_Settings bint_7401Settings =
{
	BINT_P_7401_SetInt,
	BINT_P_7401_ClearInt,
	BINT_P_7401_SetMask,
	BINT_P_7401_ClearMask,
	BINT_P_7401_ReadMask,
	BINT_P_7401_ReadStatus,	
	bint_7401A0,
#if BCHP_CHIP == 7403
	"7403"
#elif BCHP_CHIP == 7401
	"7401"
#endif
};

static void BINT_P_7401_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	switch( baseAddr )
	{
	BINT_P_STANDARD_L2_CASES
		BREG_Write32( regHandle, baseAddr + BINT_P_STD_SET, 1ul << shift );
		break;
	default:
		/* Only standard L2 interrupts support setting of interrupts */
		break;
	}
}

static void BINT_P_7401_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
		BINT_P_UHF_CASES
			/* Interrupt is cleared twice for UHF interrupt. 
			   Richa needs to look into this for real solution.
			   BREG_Write32( regHandle, baseAddr + BINT_P_UHF_STATUS, 1ul<<shift); */
			break;
		BINT_P_UPGSC_CASES
		BINT_P_IRQ0_CASES
			/* Has to be cleared at the source */
			break;
		BINT_P_XPT_MSG_ERR_CASES
			/* Write 0 to clear the int bit. Writing 1's are ingored. */
			BREG_Write32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS, ~( 1ul << shift ) );
			break;
		default:
			/* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
			break;
	}
}

static void BINT_P_7401_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
	BINT_P_UHF_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UHF_MASK);
		intEnable &= ~(1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_UHF_MASK, intEnable);
		break;
	BINT_P_IRQ0_CASES
		/* we need to keep uart a/b interrupts enabled */ 
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
		intEnable &= ~(1ul<<shift);
/* BINT_ALLOW_UART_A and BINT_ALLOW_UART_B allow the system to use BINT to manage UART A and B.
By default, we assume the OS or video decoder will manage the UART's for consoles.
Therefore, BINT stays out of the way. */
#ifndef BINT_ALLOW_UART_A
		intEnable |= BCHP_IRQ0_IRQSTAT_uairq_MASK;
#endif
#ifndef BINT_ALLOW_UART_B
		intEnable |= BCHP_IRQ0_IRQSTAT_ubirq_MASK;
#endif
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

	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}
}

static void BINT_P_7401_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
	BINT_P_UHF_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UHF_MASK);
		intEnable |= (1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_UHF_MASK, intEnable);
		break;
	BINT_P_IRQ0_CASES
		/* we need to keep uart a/b interrupts enabled */ 
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE);
		intEnable |= (1ul<<shift);
#ifndef BINT_ALLOW_UART_A
		intEnable |= BCHP_IRQ0_IRQSTAT_uairq_MASK;
#endif		
#ifndef BINT_ALLOW_UART_B
		intEnable |= BCHP_IRQ0_IRQSTAT_ubirq_MASK;
#endif		
		BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable );
		break;
	BINT_P_UPGSC_CASES
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

	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}
}

static uint32_t BINT_P_7401_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
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
		/* This register is not really a MASK. It is an ENABLE. */
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK );
	BINT_P_UHF_CASES
		/* In this register, 1 is "unmask". So it's effectively an ENABLE. */
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_UHF_MASK );
	BINT_P_IRQ0_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE );
	BINT_P_UPGSC_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_ENABLE );
	BINT_P_XPT_MSG_ERR_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_ENABLE );
		
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

static uint32_t BINT_P_7401_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
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
   	BINT_P_UHF_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_UHF_STATUS );
	BINT_P_IRQ0_CASES
		/* we need to keep uart a/b interrupts enabled, and mask them for application */ 
		{
		uint32_t val = BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS );
#ifndef BINT_ALLOW_UART_A
		val &= ~(BCHP_IRQ0_IRQSTAT_uairq_MASK);
#endif
#ifndef BINT_ALLOW_UART_B
		val &= ~(BCHP_IRQ0_IRQSTAT_ubirq_MASK);
#endif
		return val;
		}
	BINT_P_UPGSC_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_STATUS );
	BINT_P_XPT_MSG_ERR_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS );

	BINT_P_STANDARD_L2_CASES /* if BINT_IS_STANDARD is set appropriately, this will never happen */
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

#if BCHP_CHIP == 7401
const BINT_Settings *BINT_7401_GetSettings( void )
#else
const BINT_Settings *BINT_7403_GetSettings( void )
#endif
{
	return &bint_7401Settings;
}


static uint32_t GetRaveIntEnableOffset( 
	uint32_t BaseAddr 
	)
{
	uint32_t EnableAddr = 0;

	switch( BaseAddr )
	{
	    case BCHP_XPT_RAVE_INT_CX0: EnableAddr =  BCHP_XPT_RAVE_CX0_AV_INTERRUPT_ENABLES; break;
	    case BCHP_XPT_RAVE_INT_CX1: EnableAddr =  BCHP_XPT_RAVE_CX1_AV_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_CX2: EnableAddr =  BCHP_XPT_RAVE_CX2_AV_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_CX3: EnableAddr =  BCHP_XPT_RAVE_CX3_AV_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_CX4: EnableAddr =  BCHP_XPT_RAVE_CX4_AV_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_CX5: EnableAddr =  BCHP_XPT_RAVE_CX5_AV_INTERRUPT_ENABLES; break;

#if (BCHP_CHIP == 7401 && BCHP_VER >= BCHP_VER_B0) || (BCHP_CHIP == 7403)
	    case BCHP_XPT_RAVE_INT_CX6: EnableAddr =  BCHP_XPT_RAVE_CX6_AV_INTERRUPT_ENABLES; break;
	    case BCHP_XPT_RAVE_INT_CX7: EnableAddr =  BCHP_XPT_RAVE_CX7_AV_INTERRUPT_ENABLES; break;
	    case BCHP_XPT_RAVE_INT_CX8: EnableAddr =  BCHP_XPT_RAVE_CX8_AV_INTERRUPT_ENABLES; break;
	    case BCHP_XPT_RAVE_INT_CX9: EnableAddr =  BCHP_XPT_RAVE_CX9_AV_INTERRUPT_ENABLES; break;

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
#else	    	
	    case BCHP_XPT_RAVE_INT_RC0: EnableAddr =  BCHP_XPT_RAVE_RC0_REC_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_RC1: EnableAddr =  BCHP_XPT_RAVE_RC1_REC_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_RC2: EnableAddr =  BCHP_XPT_RAVE_RC2_REC_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_RC3: EnableAddr =  BCHP_XPT_RAVE_RC3_REC_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_RC4: EnableAddr =  BCHP_XPT_RAVE_RC4_REC_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_RC5: EnableAddr =  BCHP_XPT_RAVE_RC5_REC_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_RC6: EnableAddr =  BCHP_XPT_RAVE_RC6_REC_INTERRUPT_ENABLES; break;	
	    case BCHP_XPT_RAVE_INT_RC7: EnableAddr =  BCHP_XPT_RAVE_RC7_REC_INTERRUPT_ENABLES; break;	
#endif
		default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}

	return EnableAddr;
}

#if 0
/* unused static functions */
uint32_t BINT_P_7401_ReadTimer( BREG_Handle regHandle )
{
	return BREG_Read32(regHandle, BCHP_TIMER_TIMER0_STAT) & BCHP_TIMER_TIMER0_STAT_COUNTER_VAL_MASK ;
}

uint32_t BINT_P_7401_GetElapsedTime( BREG_Handle regHandle, uint32_t ulTimerStart )
{
	uint32_t ulTimerCur = BINT_P_7401_ReadTimer(regHandle);
	uint32_t ulTimerMax = BREG_Read32(regHandle, BCHP_TIMER_TIMER0_CTRL) & 
	                      BCHP_TIMER_TIMER3_CTRL_TIMEOUT_VAL_MASK;
	uint32_t ulTimerElapsed;

	if (ulTimerCur < ulTimerStart)
	{
		ulTimerElapsed = ((ulTimerMax - ulTimerStart) + ulTimerCur);
	}
	else
	{
		ulTimerElapsed = (ulTimerCur - ulTimerStart);
	}

	return (ulTimerElapsed / BINT_P_STAT_TIMER_TICKS_PER_USEC);
}
#endif

