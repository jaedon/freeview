/***************************************************************************
 *    Copyright (c) 2003-2006, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_7438.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/26/06 3:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/7438/bint_7438.c $
 * 
 * Hydra_Software_Devel/3   5/26/06 3:12p albertl
 * PR21392:  Removed remaining code accessing timers directly.
 * 
 * Hydra_Software_Devel/2   4/4/06 5:38p tdo
 * PR 20634: Add missing interrupt status case for M2MC_1_L2_CPU
 * 
 * Hydra_Software_Devel/1   3/7/06 3:17p jasonh
 * PR 19565: Initial version.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bint_7438.h"

/* Include interrupt definitions from RDB */
#include "bchp_hif_cpu_intr1.h"
#include "bchp_audio_inth.h"
#include "bchp_ifd_l2.h"
#include "bchp_rfm_l2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_2.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_hdmi_intr2.h"
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
#include "bchp_m2mc_1_l2.h"

BDBG_MODULE(interruptinterface_7438);

#define BINT_P_STD_STATUS		0x00
#define BINT_P_STD_SET			0x04
#define BINT_P_STD_CLEAR		0x08
#define BINT_P_STD_MASK_STATUS	0x0C
#define BINT_P_STD_MASK_SET		0x10
#define BINT_P_STD_MASK_CLEAR	0x14

#define BINT_P_STANDARD_L2_CASES \
	case BCHP_AUDIO_INTH_R5F_STATUS: \
	case BCHP_IFD_L2_CPU_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_HDMI_INTR2_CPU_STATUS: \
	case BCHP_VDEC_L2_0_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_2_R5F_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_RFM_L2_CPU_STATUS: \
	case BCHP_MTP_L2_1_CPU_STATUS: \
	case BCHP_M2MC_1_L2_CPU_STATUS: \
	case BCHP_HIF_INTR2_CPU_STATUS:

#define BINT_P_XPT_STATUS			0x00
#define BINT_P_XPT_ENABLE			0x04

#define BINT_P_XPT_STATUS_CASES \
	case BCHP_XPT_INT_INTR_STATUS_REG: \
	case BCHP_XPT_INT_INTR_STATUS2_REG: \
	case BCHP_XPT_INT_INTR_STATUS3_REG: \
	case BCHP_XPT_INT_INTR_STATUS4_REG: \
	case BCHP_XPT_INT_INTR_STATUS5_REG:	\
	case BCHP_XPT_INT_INTR_STATUS6_REG:


#define BINT_P_XPT_BUF_STATUS		0x00
#define BINT_P_XPT_BUF_ENABLE		0x08

#define BINT_P_XPT_BUF_CASES \
	case BCHP_XPT_INT_BUF_OVFL_REG_LO: \
	case BCHP_XPT_INT_BUF_OVFL_REG_HI: \
	case BCHP_XPT_INT_BUF_DAT_RDY_REG_LO: \
	case BCHP_XPT_INT_BUF_DAT_RDY_REG_HI:

#define BINT_P_UPG_ENABLE		0x00
#define BINT_P_UPG_STATUS		0x04

#define BINT_P_UPG_CASES \
	case BCHP_IRQ0_IRQEN: \
	case BCHP_SCIRQ0_SCIRQEN:

#define BINT_P_TIMER_STATUS		0x00
#define BINT_P_TIMER_MASK		0x04

#define BINT_P_TIMER_CASES \
	case BCHP_TIMER_TIMER_IS:

#define BINT_P_STAT_TIMER_TICKS_PER_USEC 27

static void BINT_P_7438_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7438_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7438_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void BINT_P_7438_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_7438_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_7438_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );

#if ( BCHP_CHIP == 7438 ) && ( BCHP_VER >= BCHP_VER_A0 )
static const BINT_P_IntMap bint_7438A0[] =
{
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT,        BCHP_HIF_INTR2_CPU_STATUS,       0                    ,"HIF"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS_REG,    0                    ,"XPT"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS2_REG,   0                    ,"XPT2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS4_REG,   0                    ,"XPT4"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS5_REG,   0                    ,"XPT5"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT, BCHP_XPT_INT_INTR_STATUS6_REG,   0                    ,"XPT6"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,   BCHP_XPT_INT_BUF_OVFL_REG_LO,    BINT_DONT_PROCESS_L2 ,"XPT_OVFL_LO"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,   BCHP_XPT_INT_BUF_OVFL_REG_HI,    BINT_DONT_PROCESS_L2 ,"XPT_OVFL_HI"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,    BCHP_XPT_INT_BUF_DAT_RDY_REG_LO, BINT_DONT_PROCESS_L2 ,"XPT_MSG_LO"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_CPU_INTR_SHIFT,    BCHP_XPT_INT_BUF_DAT_RDY_REG_HI, BINT_DONT_PROCESS_L2 ,"XPT_MSG_HI"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_ICAM_CPU_INTR_SHIFT,   BCHP_XPT_INT_INTR_STATUS3_REG,   0                    ,"XPT_ICAM"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BACH_CPU_INTR_SHIFT,       BCHP_AUDIO_INTH_R5F_STATUS,      0                    ,"BACH"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_IFD_CPU_INTR_SHIFT,        BCHP_IFD_L2_CPU_STATUS,          0                    ,"IFD"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_GFX_CPU_INTR_SHIFT,        BCHP_GFX_L2_CPU_STATUS,          0x380                ,"GFX"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HDMI_CPU_INTR_SHIFT,       BCHP_HDMI_INTR2_CPU_STATUS,      0                    ,"HDMI"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VDEC_CPU_INTR_SHIFT,       BCHP_VDEC_L2_0_CPU_STATUS,       0                    ,"VDEC"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT,        BCHP_VIDEO_ENC_INTR2_CPU_STATUS, 0                    ,"VEC"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_CPU_INTR_SHIFT,       BCHP_BVNB_INTR2_CPU_STATUS,      0                    ,"BVNB"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_0_SHIFT,     BCHP_BVNF_INTR2_0_R5F_STATUS,    0                    ,"BVNF_0"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT,     BCHP_BVNF_INTR2_1_R5F_STATUS,    0                    ,"BVNF_1"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_2_SHIFT,     BCHP_BVNF_INTR2_2_R5F_STATUS,    0                    ,"BVNF_2"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_3_SHIFT,     BCHP_BVNF_INTR2_5_R5F_STATUS,    0                    ,"BVNF_3"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_RFM_CPU_INTR_SHIFT,        BCHP_RFM_L2_CPU_STATUS,          0                    ,"RFM"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT,        BCHP_IRQ0_IRQEN,                 0xFFFFFE00           ,"UPG"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_BSC_CPU_INTR_SHIFT,    BCHP_IRQ0_IRQEN,                 0xF0FFFFFF           ,"BSC"},

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SPI_CPU_INTR_SHIFT,    BCHP_IRQ0_IRQEN,                 0xFFEFFFFF           ,"SPI"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SC_CPU_INTR_SHIFT,     BCHP_SCIRQ0_SCIRQEN,             0                    ,"SC"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_MTP1_CPU_INTR_SHIFT,       BCHP_MTP_L2_1_CPU_STATUS,        0                    ,"MTP1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_M2MC_1_CPU_INTR_SHIFT,     BCHP_M2MC_1_L2_CPU_STATUS,       0                    ,"M2MC1"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_TMR_CPU_INTR_SHIFT,    BCHP_TIMER_TIMER_IS,             0                    ,"TMR"},

	{ -1, 0, 0, NULL }
};
#endif

static const BINT_Settings bint_7438Settings =
{
	BINT_P_7438_SetInt,
	BINT_P_7438_ClearInt,
	BINT_P_7438_SetMask,
	BINT_P_7438_ClearMask,
	BINT_P_7438_ReadMask,
	BINT_P_7438_ReadStatus,
#if ( BCHP_CHIP == 7438 ) && ( BCHP_VER >= BCHP_VER_A0 )
	bint_7438A0,
#else
#error BINT: Unknown chip.  Port required.
#endif
	"7438"
};

static void BINT_P_7438_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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

static void BINT_P_7438_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
	default:
		/* Other types of interrupts do not support clearing of interrupts (condition must be cleared) */
		break;
	}
}

static void BINT_P_7438_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
	case BCHP_IRQ0_IRQEN:
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we should never be touching this bit in the L2 interrupt manager.
		*/
		BDBG_ASSERT( shift != BCHP_IRQ0_IRQSTAT_uarta_irq_SHIFT );
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

static void BINT_P_7438_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
	BINT_P_UPG_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we should never be touching this bit in the L2 interrupt manager.
		*/
		BDBG_ASSERT( shift != BCHP_IRQ0_IRQSTAT_uarta_irq_SHIFT );

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

static uint32_t BINT_P_7438_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
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
	BINT_P_UPG_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager
		*/
		return ~(BREG_Read32( regHandle, baseAddr + BINT_P_UPG_ENABLE )) | BCHP_IRQ0_IRQSTAT_uarta_irq_MASK;
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

static uint32_t BINT_P_7438_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
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
	BINT_P_UPG_CASES
		/*
			The BCHP_IRQ0_IRQSTAT_uarta_irq_MASK is a special bit that is actually a L1 mask.  Therefore
			we always want to ignore it in the L2 interrupt manager
		*/
		return BREG_Read32( regHandle, baseAddr + BINT_P_UPG_STATUS ) & ~BCHP_IRQ0_IRQSTAT_uarta_irq_MASK;
	default:
		/* Unhandled interrupt base address */
		BDBG_ASSERT( false );
		return 0;
	}
}

const BINT_Settings *BINT_7438_GetSettings( void )
{
	return &bint_7438Settings;
}

/* End of file */
