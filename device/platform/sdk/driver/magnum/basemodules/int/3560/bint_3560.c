/***************************************************************************
 *	   Copyright (c) 2003-2006, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_3560.c $
 * $brcm_Revision: Hydra_Software_Devel/17 $
 * $brcm_Date: 8/4/06 12:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/3560/bint_3560.c $
 * 
 * Hydra_Software_Devel/17   8/4/06 12:46p pntruong
 * PR22572: Need initial 93563 appframework bringup.
 *
 * Hydra_Software_Devel/16   5/26/06 3:11p albertl
 * PR21392:  Removed remaining code accessing timers directly.
 *
 * Hydra_Software_Devel/15   2/1/06 4:40p pntruong
 * PR17778: Added phoenix interrupt.
 *
 * Hydra_Software_Devel/14   11/18/05 3:58p jasonh
 * PR 18192: Merging B0 changes to main-line.
 *
 * Hydra_Software_Devel/PR17778/2   11/16/05 1:24p pntruong
 * PR17778: Disable upg run away interrupts.
 *
 * Hydra_Software_Devel/PR17778/1   11/4/05 4:21p pntruong
 * PR17778: B0: HDMI input required.
 *
 * Hydra_Software_Devel/Refsw_93560b0_bringup/2   11/4/05 9:16p zhangyi
 * Workaround for B0 upg bug.
 *
 * Hydra_Software_Devel/Refsw_93560b0_bringup/1   11/3/05 11:07a zhangyi
 * support 3560B0
 *
 * Hydra_Software_Devel/13   5/27/05 10:10p btan
 * PR 15680: Added smartcard support for 3560.
 *
 * Hydra_Software_Devel/12   5/6/05 6:44p rgreen
 * PR14494: Add 3560 Support
 * Enable HDMI Rx Interrupts
 *
 * Hydra_Software_Devel/11   5/4/05 10:12a rgreen
 * PR15055: Remove duplicate check in comments
 *
 * Hydra_Software_Devel/10   5/3/05 6:50p rgreen
 * PR15055: Add 3560 HDMI Rx Support
 *
 * Hydra_Software_Devel/9   4/6/05 2:05p albertl
 * PR10596: Updated chip specific settings with NULL function pointer
 * placeholder to reflect change in BINT_Settings interface.
 *
 * Hydra_Software_Devel/8   3/22/05 9:27p bandrews
 * PR13974: Added new audio interrupt l2 register
 *
 * Hydra_Software_Devel/7   3/21/05 10:26p vsilyaev
 * PR 14240: Fixed UPG IRQ0
 *
 * Hydra_Software_Devel/6   3/21/05 10:54a dlwin
 * PR 14240: More mods. found from debugging.
 *
 * Hydra_Software_Devel/5   3/18/05 2:45p gmullen
 * PR14240: Removed SUN_L2_R5F_STATUS from int map. Not enabled in the
 * user mode driver, and not used on other Magnum stacks.
 *
 * Hydra_Software_Devel/4   3/16/05 7:38p dlwin
 * PR 14240: Added fixes for problems found during debug UPG devices.
 *
 * Hydra_Software_Devel/3   3/16/05 2:53p vsilyaev
 * PR 14413:  Fixed compile problems for 3560.
 *
 * Hydra_Software_Devel/2   3/15/05 8:31p vsilyaev
 * PR 14413:  Merged from Devel_93560 branch
 *
 * Hydra_Software_Devel/Devel_93560_A0/5   3/9/05 9:28a dlwin
 * PR 14240: More changes to support 3560.
 *
 * Hydra_Software_Devel/Devel_93560_A0/4   3/7/05 6:33p dlwin
 * PR 14240: Modified code to allow compliing on 3560 platform
 *
 * Hydra_Software_Devel/Devel_93560_A0/1   3/3/05 12:07p vinwang
 * PR14240: New for bcm93560
 *
 * Hydra_Software_Devel/35   11/19/04 2:27p gmullen
 * PR13314: Added STATUS6 register to XPT interrupt list.
 *
 * Hydra_Software_Devel/34   10/5/04 4:36p cnovak
 * PR12604: Correct mask logic for timer interrupts.
 *
 * Hydra_Software_Devel/33   10/4/04 4:39p cnovak
 * PR12604: Add timer interrupt support.
 *
 * Hydra_Software_Devel/32   8/20/04 10:11a marcusk
 * PR12377: Mask soft modem interrupts
 *
 * Hydra_Software_Devel/31   6/1/04 11:12a brianlee
 * PR11291: Modify UPG int mask for B0 to handle KBD3.
 *
 * Hydra_Software_Devel/30   3/23/04 12:19p vsilyaev
 * PR10201: Added support for BVNF_CPU_INTR_2 interrupts.
 *
 * Hydra_Software_Devel/29   2/3/04 8:38p vsilyaev
 * PR 9606: Removed assert from the SmartCard interrupts.
 *
 * Hydra_Software_Devel/28   2/2/04 2:56p btan
 * PR 9570: Fixed NDS ICAM interrupt. Used the RDB names for the L1
 * interrupt names.
 *
 * Hydra_Software_Devel/27   1/30/04 5:18p syang
 * PR 9541: added BCHP_HIF_INTR2_CPU_STATUS into the define of
 * BINT_P_STANDARD_L2_CASES
 *
 * Hydra_Software_Devel/26   1/30/04 3:53p syang
 * PR 9541: added entry BCHP_HIF_INTR2_CPU_STATUS to bint_3560A0
 *
 * Hydra_Software_Devel/25   1/5/04 4:26p marcusk
 * PR9117: Updated to support PI provided L2 interrupt handler (for
 * transport message and overflow interrupts).  Updated documentation.
 *
 * Hydra_Software_Devel/24   12/29/03 4:16p marcusk
 * PR9117: Updated with changes required to support interrupt ids rather
 * than strings.
 *
 * Hydra_Software_Devel/23   12/23/03 11:24a nissen
 * PR9078: Reduced interrupt name strings for GFX.
 *
 * Hydra_Software_Devel/19   12/19/03 3:57p aram
 * PR9078: reduced the interrupt string name of audio dec
 *
 * Hydra_Software_Devel/18   12/18/03 2:37p marcusk
 * PR8985: Removed bint_priv.h since it is no longer needed.
 *
 * Hydra_Software_Devel/17   12/18/03 2:08p marcusk
 * PR8985: Refactored to use single ISR() routine. Removed reserved names.
 * Placed all platform specific defines in bint_plat.h
 *
 * Hydra_Software_Devel/16   12/16/03 6:56p vsilyaev
 * PR 8983: In 3560 mask is inverted (0 - enable, 1 - disable)
 *
 * Hydra_Software_Devel/15   12/16/03 6:02p marcusk
 * PR8983: Properly check interrupt mask before processing an interrupt.
 *
 * Hydra_Software_Devel/15   12/16/03 6:00p marcusk
 * PR8983: Updated to properly check mask before qualifying and interrupt
 * to be processed.
 *
 * Hydra_Software_Devel/14   12/2/03 3:51p sri
 * added MTP1 and MPT2 interrupts
 *
 * Hydra_Software_Devel/13   11/21/03 11:21a nissen
 * Added support for GFX interrupts.
 *
 * Hydra_Software_Devel/12   9/30/03 7:18p rgreen
 * Add HDMI Interrupts
 *
 * Hydra_Software_Devel/11   9/23/03 5:52p pntruong
 * Removed the 3560_ from header filename generated from RDB.
 *
 * Hydra_Software_Devel/10   9/19/03 11:50a dlwin
 * Removed warning generated by 'Wall'.
 *
 * Hydra_Software_Devel/9   9/16/03 10:29a marcusk
 * Updated to comply with DocJet requirements. Fixes for PR8055.
 *
 * Hydra_Software_Devel/8   9/5/03 1:39p hongtaoz
 * added interrupts for VDC and VBI.
 *
 * Hydra_Software_Devel/7   9/4/03 12:50p jasonh
 * Renamed bcm3560 to 3560.
 *
 * Hydra_Software_Devel/6   8/29/03 12:02p dlwin
 * - Added more reserved bits for IFD interrupt map. - Changed RFM
 * interrupt names to match RDB doc.
 *
 * Hydra_Software_Devel/5   8/28/03 5:05p marcusk
 * Added interrupts for IFD and RFM.
 *
 * Hydra_Software_Devel/4   8/26/03 10:25a marcusk
 * Updated to return handle settings (so that it can be used directly when
 * creating an interrupt handle).
 *
 * Hydra_Software_Devel/3   8/25/03 3:17p aram
 * fixed a compile warning
 *
 * Hydra_Software_Devel/2   8/13/03 1:07p marcusk
 * Updated to list all L1 interrupts.
 *
 * Hydra_Software_Devel/1   8/5/03 3:16p marcusk
 * Initial version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bint_3560.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"
#include "bchp_audio_inth.h"
#include "bchp_audio_inth_dbg.h"
#include "bchp_ifd_l2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_2.h"
#include "bchp_bvnf_intr2_5.h"
#if BCHP_VER == BCHP_VER_A0
#include "bchp_hdmi_intr2.h"
#include "bchp_hdmi_rx_interrupts.h"
#else
#include "bchp_int_id_hdmi_rx_intr2.h"
#include "bchp_sparrow_intr2.h"
#endif
#include "bchp_vdec_l2_0.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_gfx_l2.h"
#include "bchp_mtp_l2_1.h"
#include "bchp_xpt_int.h"
#include "bchp_irq0.h"
#include "bchp_scirq0.h"
#include "bchp_hif_intr2.h"
#include "bchp_bvnf_intr2_2.h"
#include "bchp_timer.h"
#include "bchp_sun_l2.h"

BDBG_MODULE(interruptinterface_3560);

#define BINT_P_STD_STATUS		0x00
#define BINT_P_STD_SET			0x04
#define BINT_P_STD_CLEAR		0x08
#define BINT_P_STD_MASK_STATUS	0x0C
#define BINT_P_STD_MASK_SET		0x10
#define BINT_P_STD_MASK_CLEAR	0x14

#define BINT_P_IRQ0_ENABLE		(-4)
#define BINT_P_IRQ0_STATUS		0x00

#if BCHP_VER == BCHP_VER_A0
#define BINT_P_STANDARD_L2_CASES \
	case BCHP_AUDIO_INTH_R5F_STATUS: \
	case BCHP_AUDIO_INTH_DBG_R5F_STATUS: \
	case BCHP_IFD_L2_CPU_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_HDMI_INTR2_CPU_STATUS: \
	case BCHP_HDMI_RX_INTERRUPTS_CPU_STATUS: \
	case BCHP_VDEC_L2_0_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_2_R5F_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_MTP_L2_1_CPU_STATUS: \
	case BCHP_HIF_INTR2_CPU_STATUS:
#else
#define BINT_P_STANDARD_L2_CASES \
	case BCHP_AUDIO_INTH_R5F_STATUS: \
	case BCHP_AUDIO_INTH_DBG_R5F_STATUS: \
	case BCHP_IFD_L2_CPU_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_SPARROW_INTR2_CPU_STATUS: \
	case BCHP_HDMI_RX_INTR2_CPU_STATUS: \
	case BCHP_VDEC_L2_0_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_2_R5F_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_MTP_L2_1_CPU_STATUS: \
	case BCHP_HIF_INTR2_CPU_STATUS:

#endif
#define BINT_P_XPT_STATUS			0x00
#define BINT_P_XPT_ENABLE			0x04

#define BINT_P_XPT_STATUS_CASES \
	case BCHP_XPT_INT_INTR_STATUS_REG: \
	case BCHP_XPT_INT_INTR_STATUS2_REG: \
	case BCHP_XPT_INT_INTR_STATUS4_REG: \
	case BCHP_XPT_INT_INTR_STATUS5_REG:	\
	case BCHP_XPT_INT_INTR_STATUS6_REG:

#define BINT_P_XPT_BUF_STATUS		0x00
#define BINT_P_XPT_BUF_ENABLE		0x08

#define BINT_P_XPT_BUF_CASES \
	case BCHP_XPT_INT_BUF_OVFL_REG_LO: \
	case BCHP_XPT_INT_BUF_DAT_RDY_REG_LO:

#define BINT_P_UPG_ENABLE		0x00
#define BINT_P_UPG_STATUS		0x04

#define BINT_P_UPG_CASES \
	case BCHP_SCIRQ0_SCIRQEN:

/* This is to handle the new UPG interrupt scheme on 3560 */
#define	BINT_P_IRQ0_CASES \
	case BCHP_IRQ0_ANT_IRQSTAT: \
	case BCHP_IRQ0_GPIO_IRQSTAT: \
	case BCHP_IRQ0_SPI_IRQSTAT: \
	case BCHP_IRQ0_IRQSTAT: \
	case BCHP_IRQ0_BSC_IRQSTAT:

#define BINT_P_TIMER_STATUS		0x00
#define BINT_P_TIMER_MASK		0x04

#define BINT_P_TIMER_CASES \
	case BCHP_TIMER_TIMER_IS:

static void BINT_P_3560_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_3560_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_3560_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_3560_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_3560_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_3560_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );

static const BINT_P_IntMap bint_3560[] =
{
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT, BCHP_HIF_INTR2_CPU_STATUS,              0                    , "HIF"     },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS_REG,    0                    , "XPT"     },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS2_REG,   0                    , "XPT2"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS4_REG,   0                    , "XPT4"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS5_REG,   0                    , "XPT5"    },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS6_REG,   0                    , "XPT6"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,   BCHP_XPT_INT_BUF_OVFL_REG_LO,    BINT_DONT_PROCESS_L2 , "OVFL"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,    BCHP_XPT_INT_BUF_DAT_RDY_REG_LO, BINT_DONT_PROCESS_L2 , "MSG"     },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VDEC_CPU_INTR_SHIFT,       BCHP_VDEC_L2_0_CPU_STATUS,       0                    , "VDEC"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_MTP_CPU_INTR_SHIFT,        BCHP_MTP_L2_1_CPU_STATUS,        0                    , "MTP"     },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_ANT_INTR_SHIFT,        BCHP_IRQ0_ANT_IRQSTAT,           0                    , "ANT"     },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_GPIO_INTR_SHIFT,       BCHP_IRQ0_GPIO_IRQSTAT,          0                    , "GPIO"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SPI_CPU_INTR_SHIFT,    BCHP_IRQ0_SPI_IRQSTAT,           0                    , "SPI"     },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_BSC_CPU_INTR_SHIFT,    BCHP_IRQ0_BSC_IRQSTAT,           0                    , "BSC"     },
#if BCHP_VER == BCHP_VER_A0 	/*disable UPG_CPU interrupt on B0*/
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT,        BCHP_IRQ0_IRQSTAT,               0xFFFFFF78           , "IRQ0"    },
#endif
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_TMR_CPU_INTR_SHIFT,    BCHP_TIMER_TIMER_IS,             0                    , "TIMER"   },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SC_CPU_INTR_SHIFT,     BCHP_SCIRQ0_SCIRQEN,             0                    , "SCIRQ0"  },


	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_3_SHIFT,     BCHP_BVNF_INTR2_5_R5F_STATUS,    0                    , "BVNF5"   },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_2_SHIFT,     BCHP_BVNF_INTR2_2_R5F_STATUS,    0                    , "BVNF2"   },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT,     BCHP_BVNF_INTR2_1_R5F_STATUS,    0                    , "BVNF1"   },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_0_SHIFT,     BCHP_BVNF_INTR2_0_R5F_STATUS,    0                    , "BVNF0"   },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_CPU_INTR_SHIFT,       BCHP_BVNB_INTR2_CPU_STATUS,      0                    , "BVNB"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT,        BCHP_VIDEO_ENC_INTR2_CPU_STATUS, 0                    , "VEC"     },
#if BCHP_VER == BCHP_VER_A0
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_CPU_INTR_SHIFT,       BCHP_HDMI_INTR2_CPU_STATUS,      0                    , "HDMI"    },
#else
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DVO_CPU_INTR_SHIFT,       BCHP_SPARROW_INTR2_CPU_STATUS,    0                    , "DVO"     },
#endif
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT,        BCHP_GFX_L2_CPU_STATUS,          0x380                , "GFX"     },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_IFD_CPU_INTR_SHIFT,        BCHP_IFD_L2_CPU_STATUS,          0                    , "IFD"     },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BACH_CPU_INTR_SHIFT,       BCHP_AUDIO_INTH_R5F_STATUS,      0                    , "BACH"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BACH_CPU_INTR_SHIFT,       BCHP_AUDIO_INTH_DBG_R5F_STATUS,  0                    , "BACH_DBG"},
#if BCHP_VER == BCHP_VER_A0
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PHX_CPU_INTR_SHIFT+32,     BCHP_HDMI_RX_INTERRUPTS_CPU_STATUS, 0                 , "HDMI_RX" },
#else
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PHX_CPU_INTR_SHIFT+32,     BCHP_HDMI_RX_INTR2_CPU_STATUS,      0                 , "HDMI_RX" },
#endif
	{ -1, 0, 0, "Invalid" }
};

static const BINT_Settings bint_3560Settings =
{
	BINT_P_3560_SetInt,
	BINT_P_3560_ClearInt,
	BINT_P_3560_SetMask,
	BINT_P_3560_ClearMask,
	BINT_P_3560_ReadMask,
	BINT_P_3560_ReadStatus,
	bint_3560,
	"3560"
};

static void BINT_P_3560_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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

static void BINT_P_3560_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	switch( baseAddr )
	{
	BINT_P_STANDARD_L2_CASES
		BREG_Write32( regHandle, baseAddr + BINT_P_STD_CLEAR, 1ul<<shift);
		break;
	BINT_P_XPT_STATUS_CASES
		BREG_Write32( regHandle, baseAddr + BINT_P_XPT_STATUS, ~(1ul<<shift));
		break;
	BINT_P_XPT_BUF_CASES
		BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_STATUS, ~(1ul<<shift));
		break;
	BINT_P_TIMER_CASES
		BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_STATUS, 1ul<<shift);
		break;
	BINT_P_IRQ0_CASES
		/* Has to cleared at the source */
		break;
	default:
		/* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
		break;
	}
}

static void BINT_P_3560_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	uint32_t intEnable;

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
		BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, 0 );
		break;
	case BCHP_IRQ0_IRQEN:
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we should never be touching this bit in the L2 interrupt manager.
		*/
		BDBG_ASSERT( shift != BCHP_IRQ0_IRQSTAT_ua_SHIFT );
		/* fell through */
	case BCHP_SCIRQ0_SCIRQEN:

		/* TODO: The linux kernel may also use this interrupt register, so we must disable interrupts */
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UPG_ENABLE );
		intEnable &= ~(1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_UPG_ENABLE, intEnable );
		break;
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}
}

static void BINT_P_3560_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
{
	uint32_t intEnable;

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
	BINT_P_XPT_BUF_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE);
		intEnable |= 1ul<<shift;
		BREG_Write32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE, intEnable);
		break;
	BINT_P_IRQ0_CASES
		intEnable = 1;
		BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable );
		break;
	BINT_P_UPG_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we should never be touching this bit in the L2 interrupt manager.
		*/
		BDBG_ASSERT( shift != BCHP_IRQ0_IRQSTAT_ua_SHIFT );

		/* TODO: The linux kernel may also use this interrupt register, so we must disable interrupts */
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_UPG_ENABLE );
		intEnable |= 1ul<<shift;
		BREG_Write32( regHandle, baseAddr + BINT_P_UPG_ENABLE, intEnable );
		break;
	BINT_P_TIMER_CASES
		intEnable = BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK );
		intEnable |= (1ul<<shift);
		BREG_Write32( regHandle, baseAddr + BINT_P_TIMER_MASK, intEnable );
		break;
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		break;
	}
}

static uint32_t BINT_P_3560_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
{
	switch( baseAddr )
	{
	BINT_P_STANDARD_L2_CASES
		return BREG_Read32(regHandle, baseAddr + BINT_P_STD_MASK_STATUS);
	BINT_P_XPT_STATUS_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_ENABLE );
	BINT_P_XPT_BUF_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_ENABLE );
	BINT_P_TIMER_CASES
		return ~BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_MASK );
	BINT_P_IRQ0_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE );
	BINT_P_UPG_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager
		*/
		return ~(BREG_Read32( regHandle, baseAddr + BINT_P_UPG_ENABLE )) | BCHP_IRQ0_IRQSTAT_ua_MASK;
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

static uint32_t BINT_P_3560_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
{
	switch( baseAddr )
	{
	BINT_P_STANDARD_L2_CASES
		return BREG_Read32(regHandle, baseAddr + BINT_P_STD_STATUS);
	BINT_P_XPT_STATUS_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_STATUS );
	BINT_P_XPT_BUF_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_BUF_STATUS );
	BINT_P_TIMER_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_TIMER_STATUS );
	BINT_P_IRQ0_CASES
		return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS );
		break;
	BINT_P_UPG_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager
		*/
		return BREG_Read32( regHandle, baseAddr + BINT_P_UPG_STATUS ) & ~BCHP_IRQ0_IRQSTAT_ua_MASK;
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

const BINT_Settings *BINT_3560_GetSettings( void )
{
	return &bint_3560Settings;
}
