/***************************************************************************
 *	   Copyright (c) 2003-2007, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bint_3573.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/9/07 10:34a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/int/3573/bint_3573.c $
 * 
 * Hydra_Software_Devel/1   10/9/07 10:34a pntruong
 * PR34848: Added stub for compiling.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bint_3573.h"

/* Include interrupt definitions from RDB */
#include "bchp_aio_inth.h"
#include "bchp_aud_dsp_inth0.h"
#include "bchp_bsp_control_intr2.h"
#include "bchp_bvnb_intr2.h"
#include "bchp_bvnf_intr2_0.h"
#include "bchp_bvnf_intr2_1.h"
#include "bchp_bvnf_intr2_3.h"
#include "bchp_bvnf_intr2_4.h"
#include "bchp_bvnf_intr2_5.h"
#include "bchp_gfx_l2.h"
#include "bchp_hdmi_rx_intr2_0.h"
#include "bchp_hdmi_rx_intr2_1.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_hif_intr2.h"
#include "bchp_ifd_l2.h"
#include "bchp_irq0.h"
#include "bchp_mtp_l2_1.h"
#include "bchp_scirq0.h"
#include "bchp_dvpo_intr2_0.h"
#include "bchp_sun_l2.h"
#include "bchp_timer.h"
#include "bchp_vdec_l2.h"
#include "bchp_video_enc_intr2.h"
#include "bchp_xpt_bus_if.h"
#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bchp_xpt_fe.h"
#include "bchp_xpt_msg.h"
#include "bchp_xpt_pb0.h"
#include "bchp_xpt_rave.h"

BDBG_MODULE(BINT);

/* Standard L2 interrupt cases */
#define BINT_P_STD_STATUS           ( 0x00 )
#define BINT_P_STD_SET              ( 0x04 )
#define BINT_P_STD_CLEAR            ( 0x08 )
#define BINT_P_STD_MASK_STATUS      ( 0x0C )
#define BINT_P_STD_MASK_SET         ( 0x10 )
#define BINT_P_STD_MASK_CLEAR       ( 0x14 )

#define BINT_P_STANDARD_L2_CASES \
	case BCHP_AIO_INTH_R5F_STATUS: \
	case BCHP_AUD_DSP_INTH0_R5F_STATUS: \
	case BCHP_BSP_CONTROL_INTR2_CPU_STATUS: \
	case BCHP_BVNB_INTR2_CPU_STATUS: \
	case BCHP_BVNF_INTR2_0_R5F_STATUS: \
	case BCHP_BVNF_INTR2_1_R5F_STATUS: \
	case BCHP_BVNF_INTR2_5_R5F_STATUS: \
	case BCHP_GFX_L2_CPU_STATUS: \
	case BCHP_HDMI_RX_INTR2_0_CPU_STATUS: \
	case BCHP_HDMI_RX_INTR2_1_CPU_STATUS: \
	case BCHP_HIF_INTR2_CPU_STATUS: \
	case BCHP_IFD_L2_CPU_STATUS: \
	case BCHP_MTP_L2_1_CPU_STATUS: \
	case BCHP_DVPO_INTR2_0_CPU_STATUS : \
	case BCHP_SUN_L2_CPU_STATUS: \
	case BCHP_VDEC_L2_CPU_STATUS: \
	case BCHP_VIDEO_ENC_INTR2_CPU_STATUS: \

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
	case BCHP_XPT_PB0_INTR:

/* Transport RAVE cases */
#define BINT_P_RAVE_STATUS			    ( 0x00 )

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
#define BINT_P_XPT_BUF_STATUS		    ( 0x00 )
#define BINT_P_XPT_BUF_ENABLE		    ( 0x10 )

#define BINT_P_XPT_BUF_CASES \
	case BCHP_XPT_MSG_BUF_OVFL_INTR_00_31: \
	case BCHP_XPT_MSG_BUF_OVFL_INTR_32_63: \
	case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_00_31: \
	case BCHP_XPT_MSG_BUF_DAT_RDY_INTR_32_63:

/* Transport message error cases */
#define BINT_P_XPT_MSG_ERR_STATUS	  ( 0x00 )
#define BINT_P_XPT_MSG_ERR_ENABLE	  ( 0x04 )

#define BINT_P_XPT_MSG_ERR_CASES \
	case BCHP_XPT_MSG_DAT_ERR_INTR:

/* Timer cases */
#define BINT_P_TIMER_STATUS		      ( 0x00 )
#define BINT_P_TIMER_ENABLE		      ( 0x04 )

#define BINT_P_TIMER_CASES \
	case BCHP_TIMER_TIMER_IS:

/* Smart Card cases */
#define BINT_P_UPGSC_ENABLE         ( 0x00 )
#define BINT_P_UPGSC_STATUS         ( 0x04 )

#define BINT_P_UPGSC_CASES \
	case BCHP_SCIRQ0_SCIRQEN: \


/* IRQ0 cases */
#define BINT_P_IRQ0_ENABLE		      ( 0 )
#define BINT_P_IRQ0_STATUS		      ( 4 )

/* case BCHP_IRQ0_UARTA_IRQEN : \ */

#define BINT_P_IRQ0_CASES \
	case BCHP_IRQ0_ANT_IRQEN: \
	case BCHP_IRQ0_GPIO_IRQEN: \
	case BCHP_IRQ0_SPI_IRQEN:

#define	BINT_P_IRQ0_IRQ_CASE \
	case BCHP_IRQ0_IRQEN:


static void     BINT_P_3573_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void     BINT_P_3573_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void     BINT_P_3573_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static void     BINT_P_3573_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift );
static uint32_t BINT_P_3573_ReadMask( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t BINT_P_3573_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr );
static uint32_t GetRaveIntEnableOffset( uint32_t BaseAddr );

static const BINT_P_IntMap bint_3573[] =
{
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VDECS_CPU_INTR_SHIFT,          BCHP_VDEC_L2_CPU_STATUS,              0,                    "VDEC"       },

	/* BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_OB_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_TER_CPU_INTR_SHIFT */

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_SUN_CPU_INTR_SHIFT,            BCHP_SUN_L2_CPU_STATUS,               0,                    "SUN"        },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SC_CPU_INTR_SHIFT,         BCHP_SCIRQ0_SCIRQEN,                  0,                    "SCIRQ0"     },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_SPI_CPU_INTR_SHIFT,        BCHP_IRQ0_SPI_IRQEN,                  0,                    "SPI"        },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_CPU_INTR_SHIFT,            BCHP_IRQ0_IRQEN,                      0,                    "UPG"        },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_UPG_TMR_CPU_INTR_SHIFT,        BCHP_TIMER_TIMER_IS,                  0,                    "TIMER"      },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_IFD_CPU_INTR_SHIFT,            BCHP_IFD_L2_CPU_STATUS,               0,                    "IFD"        },

	/* BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DS2_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DS1_CPU_INTR_SHIFT */

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_3_SHIFT,         BCHP_BVNF_INTR2_5_R5F_STATUS,         0,                    "BVNF5"      },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_1_SHIFT,         BCHP_BVNF_INTR2_1_R5F_STATUS,         0,                    "BVNF1"      },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNF_CPU_INTR_0_SHIFT,         BCHP_BVNF_INTR2_0_R5F_STATUS,         0,                    "BVNF0"      },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_BVNB_CPU_INTR_SHIFT,           BCHP_BVNB_INTR2_CPU_STATUS,           0,                    "BVNB"       },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_VEC_CPU_INTR_SHIFT,            BCHP_VIDEO_ENC_INTR2_CPU_STATUS,      0,                    "VEC"        },

	/* BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_AP_CPU_INTR_SHIFT */

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_DVO_CPU_INTR_SHIFT,            BCHP_DVPO_INTR2_0_CPU_STATUS,        0,                    "DVO"        },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_MSG_STAT_CPU_INTR_SHIFT,   BCHP_XPT_MSG_DAT_ERR_INTR,            0,                    "XPT_MSG_ERR"},
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_FE_CPU_INTR_SHIFT,         BCHP_XPT_FE_INTR_STATUS_REG,          0,                    "XPT_FE"     },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_PCR_CPU_INTR_SHIFT,        BCHP_XPT_DPCR0_INTR_STATUS_REG,       0,                    "XPT_DPCR0"  },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_PCR_CPU_INTR_SHIFT,        BCHP_XPT_DPCR1_INTR_STATUS_REG,       0,                    "XPT_DPCR1"  },

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

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,       BCHP_XPT_MSG_BUF_OVFL_INTR_00_31,     BINT_DONT_PROCESS_L2, "XPT_OVFL_0" },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_OVFL_CPU_INTR_SHIFT,       BCHP_XPT_MSG_BUF_OVFL_INTR_32_63,     BINT_DONT_PROCESS_L2, "XPT_OVFL_1" },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS_REG,      0,                    "XPT"        },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS2_REG,     0,                    "XPT2"       },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS4_REG,     0,                    "XPT4"       },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_PB0_INTR,                    0,                    "XPT_PB0"    },
	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_XPT_STATUS_CPU_INTR_SHIFT,     BCHP_XPT_BUS_IF_INTR_STATUS5_REG,     0,                    "XPT5"       },

	{ BCHP_HIF_CPU_INTR1_INTR_W0_STATUS_HIF_CPU_INTR_SHIFT,            BCHP_HIF_INTR2_CPU_STATUS,            0xFFFFF000,           "HIF"        },

	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_2_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_1_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_PCI_INTA_0_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_6_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_5_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_4_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_3_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_2_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_1_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_EXT_IRQ_0_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SPARE_3_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SPARE_2_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SPARE_1_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_SPARE_0_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_DVP_HR_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_DVP_DL_CPU_INTR_SHIFT */
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_BSP_CPU_INTR_SHIFT       + 32, BCHP_BSP_CONTROL_INTR2_CPU_STATUS,    0xFFFFFFFC,           "BSP"        },
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_RPTD_CPU_INTR_SHIFT      + 32, BCHP_AUD_DSP_INTH0_R5F_STATUS,        0,                    "RTPD"       },
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_AIO_CPU_INTR_SHIFT       + 32, BCHP_AIO_INTH_R5F_STATUS,             0,                    "AIO"        },
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_GFX_CPU_INTR_SHIFT       + 32, BCHP_GFX_L2_CPU_STATUS,               0xFFFFFFF0,           "GFX"        },
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_HDMI_RX_1_CPU_INTR_SHIFT + 32, BCHP_HDMI_RX_INTR2_1_CPU_STATUS,      0,                    "HDMI_RX_1"  },
	{ BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_HDMI_RX_0_CPU_INTR_SHIFT + 32, BCHP_HDMI_RX_INTR2_0_CPU_STATUS,      0,                    "HDMI_RX_0"  },

	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_BVNM_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_MCIF_CPU_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB20_OHCI_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB20_EHCI_INTR_SHIFT */
	/* BCHP_HIF_CPU_INTR1_INTR_W1_STATUS_USB20_BRIDGE_INTR_SHIFT */

	{ -1, 0, 0, "Invalid" }
};

static const BINT_Settings bint_3573Settings =
{
	BINT_P_3573_SetInt,
	BINT_P_3573_ClearInt,
	BINT_P_3573_SetMask,
	BINT_P_3573_ClearMask,
	BINT_P_3573_ReadMask,
	BINT_P_3573_ReadStatus,
	bint_3573,
	"3573"
};

static void BINT_P_3573_SetInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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

static void BINT_P_3573_ClearInt( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
		BINT_P_IRQ0_CASES
		BINT_P_IRQ0_IRQ_CASE
			/* Has to cleared at the source */
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

static void BINT_P_3573_SetMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
		BINT_P_IRQ0_CASES
			BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, 0 );
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

		default:
			/* Unhandled interrupt base address */
			BDBG_ASSERT( false );
			break;
	}
}

static void BINT_P_3573_ClearMask( BREG_Handle regHandle, uint32_t baseAddr, int shift )
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
		BINT_P_IRQ0_CASES
			intEnable  = 1;
			BREG_Write32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE, intEnable );
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

		default:
			/* Unhandled interrupt base address */
			BDBG_ASSERT( false );
			break;
	}
}

static uint32_t BINT_P_3573_ReadMask( BREG_Handle regHandle, uint32_t baseAddr )
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
		BINT_P_IRQ0_CASES
		BINT_P_IRQ0_IRQ_CASE
			return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_ENABLE );
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

static uint32_t BINT_P_3573_ReadStatus( BREG_Handle regHandle, uint32_t baseAddr )
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
		BINT_P_IRQ0_CASES
		BINT_P_IRQ0_IRQ_CASE
			return BREG_Read32( regHandle, baseAddr + BINT_P_IRQ0_STATUS );
		BINT_P_UPGSC_CASES
			return BREG_Read32( regHandle, baseAddr + BINT_P_UPGSC_STATUS );
		BINT_P_XPT_MSG_ERR_CASES
			return BREG_Read32( regHandle, baseAddr + BINT_P_XPT_MSG_ERR_STATUS );

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

const BINT_Settings *BINT_3573_GetSettings( void )
{
	return &bint_3573Settings;
}

/* End of file */
