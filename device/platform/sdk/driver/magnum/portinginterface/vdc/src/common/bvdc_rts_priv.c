/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_rts_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/16 $
 * $brcm_Date: 8/9/12 11:19a $
 *
 * Module Description:
 *   This module is for dynamics RTS (Real Time Schedule of memory bandwidth).
 * Thare is a compile flag BVDC_DYNAMICS_RTS for selecting which version of
 * 3563-deriviative going to be use:
 *
 *  1) BVDC_DYNAMICS_RTS == 3552
 *  2) BVDC_DYNAMICS_RTS == 3553
 *  3) BVDC_DYNAMICS_RTS == 3563 (default)
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_rts_priv.c $
 * 
 * Hydra_Software_Devel/16   8/9/12 11:19a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 * 
 * Hydra_Software_Devel/15   12/21/07 1:26p tdo
 * PR38419: Fix XPT_2 client ID.
 * 
 * Hydra_Software_Devel/14   9/27/07 6:05p pntruong
 * PR29984: Updated typo for 3552 cap/vfd values.
 *
 * Hydra_Software_Devel/13   9/24/07 7:28p pntruong
 * PR33354: Removed build warnings.
 *
 * Hydra_Software_Devel/12   8/9/07 5:18p pntruong
 * PR33348: Correctly identify hd/ed/sd in case of oversampling.
 *
 * Hydra_Software_Devel/11   8/6/07 11:08a pntruong
 * PR33348: Identify mpeg format of hd/sd/ed with curinfo->pfmt.
 *
 * Hydra_Software_Devel/10   7/27/07 6:16p pntruong
 * PR29984: Updated to new rts version 2.2.
 *
 * Hydra_Software_Devel/9   7/24/07 11:35a pntruong
 * PR33256: Allow lower capture precision to be selected.
 *
 * Hydra_Software_Devel/8   7/20/07 5:01p pntruong
 * PR33256: [VDC] Dynamical RTS settings for Component/HDMI 1080i on 3553
 * is wrong.
 *
 * Hydra_Software_Devel/7   7/3/07 6:27p pntruong
 * PR32591: Need seperate RTS table for 3563/3553/3552.
 *
 * Hydra_Software_Devel/6   6/28/07 9:14p pntruong
 * PR32503: Updated rts for oversample support.
 *
 * Hydra_Software_Devel/5   5/22/07 7:48p pntruong
 * PR29662: Added detection for dynamics rts triggers.
 *
 * Hydra_Software_Devel/4   5/3/07 7:00p pntruong
 * PR29662: Added c0 dynamics rts settings.
 *
 * Hydra_Software_Devel/3   4/4/07 3:45p syang
 * PR 29243: change VnetMode def (now use bit-fields)
 *
 * Hydra_Software_Devel/2   2/14/07 3:06p yuxiaz
 * PR 23202: Rename RTS table for ANR. Simplify code to set dynamic RTS.
 *
 * Hydra_Software_Devel/1   1/25/07 10:04a yuxiaz
 * PR 23202: Added for RTS reconfig in VDC.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bvdc_rts_priv.h"
#include "bvdc_capture_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"

#if (BVDC_P_SUPPORT_DYNAMIC_RTS)
#include "bchp_memc_0.h"
#include "bchp_memc_1.h"


BDBG_MODULE(BVDC_RTS);


/***************************************************************************
 *
 */
static void BVDC_P_Rts_SetClientInfo_isr
	( BREG_Handle                      hReg,
	  const BVDC_P_AddrDataPair        aRtsTable[],
	  uint32_t                         ulCount )
{
	uint32_t  i;

	for(i = 0; i < ulCount; i++)
	{
		BREG_Write32_isr(hReg, aRtsTable[i].ulAddr, aRtsTable[i].ulData);
	}

	return;
}


/***************************************************************************
 * RTS Tables
 */
#if ((BCHP_CHIP==3563) && (BCHP_VER >= BCHP_VER_C0))
#if (BVDC_DYNAMICS_RTS==3552)
/***************************************************************************
 * RTS Tables
 */
static const BVDC_P_AddrDataPair s_Cap0_C1[] = /* PC input RGB (CAP/VFD 10-bit 444, ANR off, MAD off) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00006302 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009b03 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C2[] = /* 1080p/720p/480p source (CAP/VFD 10-bit 422, MAD off) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x0000c502 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00011a03 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C3[] = /* 1080i source (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00018d04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00018c03 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C4[] = /* 480i source (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00018d04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00018c03 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A1[] = /* 1080p source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x0000c501 },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x0000c501 }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A2[] = /* 1080i/720p/480p source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x00018706 },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x00018706 }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A3[] = /* 480i source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x00044c1a },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x00044c1a }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A4[] = /* 480i source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x0002391a },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x0002391a }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Xpt2_X1[] = /* MPEG source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_58, 0x0031913 } /* XPT_2 */
};

static const BVDC_P_AddrDataPair s_Xpt2_X2[] = /* Non-MPEG source (HDMI/Analog) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_58, 0x00032813 } /* XPT_2 */
};

/***************************************************************************
 * Reconfigure RTS for selected window path
 *
 * All related HW block are down to change to a new setting (Cx)
 *
 * Blockout for CAP0/VFD0 will have 4 potential settings modes:
 *   Setting C1: PC input RGB (CAP/VFD 10-bit 444, ANR off, MAD off)
 *   Setting C2: 1080p/720p/480p source (CAP/VFD 8-bit 422, MAD off)
 *   Setting C3: 1080i source (CAP/VFD 8-bit 422)
 *   Setting C4: 480i source
 *
 * Blockout for ANR will have 4 potential settings modes:
 *   Setting A1: 1080p source
 *   Setting A2: 1080i/720p source
 *   Setting A3: 480i source
 *   Setting A4: 480p source
 *
 * Blockout for XPT2 will have 2 potential settings modes:
 *   Setting X1: MPEG source
 *   Setting X2: Non-MPEG source (HDMI/Analog)
 */
void BVDC_P_Rts_Reconfig_isr
	( const BVDC_P_WindowContext      *pWindow,
	  const BVDC_P_SourceContext      *pSource,
	  const BAVC_MVD_Field            *pMvdFieldData )
{
	const BVDC_P_AddrDataPair *pCapRts;
	const BVDC_P_AddrDataPair *pAnrRts;
	const BVDC_P_AddrDataPair *pXptRts;

	BSTD_UNUSED(pMvdFieldData);

	/* Need to handle dynamics RTS for: VFD/CAP/MAD/ANR/GFX/MFD */
	if(BVDC_P_WindowId_eComp0_V0 != pWindow->eId)
	{
		return;
	}

	BDBG_MSG(("3552 Dynamics RTS"));

	/* (1) Capture/Feeder RTS */
	if((!BVDC_P_VNET_USED_MAD(pWindow->stVnetMode) &&
	    !BVDC_P_VNET_USED_ANR(pWindow->stVnetMode)) &&
	   (BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	    BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	    BPXL_IS_YCbCr444_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat)))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C1]: 10-bit 444, ANR off, MAD off", pWindow->eId));
		pCapRts = s_Cap0_C1;
	}
	else if(!BVDC_P_VNET_USED_MAD(pWindow->stVnetMode) &&
	         BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C2]: 8-bit 422, MAD off", pWindow->eId));
		pCapRts = s_Cap0_C2;
	}
	else if(BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C3]: 8-bit 422", pWindow->eId));
		pCapRts = s_Cap0_C3;
	}
	else if(BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C4]: 10-bit 422", pWindow->eId));
		pCapRts = s_Cap0_C4;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C3]: 8-bit 422", pWindow->eId));
		pCapRts = s_Cap0_C3;
	}

	/* (2) Anr RTS */
	if((pSource->stCurInfo.pVdcFmt->bHd) &&
	   (pSource->stCurInfo.pVdcFmt->bProgressive) &&
	   (pSource->stCurInfo.pFmtInfo->ulWidth >= BFMT_1080P_WIDTH))
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A1]: 1080p.", pWindow->eId));
		pAnrRts = s_Anr0_A1;
	}
	else if(pSource->stCurInfo.pVdcFmt->bHd)
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A2]: HD.", pWindow->eId));
		pAnrRts = s_Anr0_A2;
	}
	else if(pSource->stCurInfo.pVdcFmt->bSd)
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A3]: SD.", pWindow->eId));
		pAnrRts = s_Anr0_A3;
	}
	else /* ED */
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A4]: ED.", pWindow->eId));
		pAnrRts = s_Anr0_A4;
	}

	/* (3) Mpeg vs Non-Mpeg. */
	if(BVDC_P_SRC_IS_MPEG(pSource->eId))
	{
		BDBG_MSG(("pWindow[%d] gfx rts mode[X1]: Mpeg source.", pWindow->eId));
		pXptRts = s_Xpt2_X1;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] gfx rts mode[X2]: Non-Mpeg source.", pWindow->eId));
		pXptRts = s_Xpt2_X2;
	}

	/* Write them out */
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pCapRts,
		sizeof(s_Cap0_C1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pAnrRts,
		sizeof(s_Anr0_A1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pXptRts,
		sizeof(s_Xpt2_X1) / sizeof(BVDC_P_AddrDataPair));

	return;
}

#elif (BVDC_DYNAMICS_RTS==3553)
/***************************************************************************
 * RTS Tables
 */
static const BVDC_P_AddrDataPair s_Cap0_C1[] = /* PC input RGB (CAP/VFD 10-bit 444, ANR off, MAD off) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00005f04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00006403 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C2[] = /* 1080p/720p/480p source (CAP/VFD 10-bit 422, MAD off) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00009d04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009703 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C3[] = /* 1080i source (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00013d04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009703 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C4[] = /* 480i source (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x0001b704 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009703 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C5[] = /* MPEG source Scaled to PIG (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x0001b704 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009703 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A1[] = /* 1080p source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x0000c501 },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x0000c501 }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A2[] = /* 1080i/720p/480p source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x00017e06 },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x00017e06 }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A3[] = /* 480i source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x0002391a },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x0002391a }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Mad0_D1[] = /* Not Scaled in the MAD */
{
	{ BCHP_MEMC_0_CLIENT_INFO_49, 0x00018d07 }, /* MAD_0_VFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_50, 0x00018d07 }, /* MAD_0_VFD_1 */
	{ BCHP_MEMC_0_CLIENT_INFO_51, 0x00018d07 }, /* MAD_0_VFD_2 */
	{ BCHP_MEMC_0_CLIENT_INFO_52, 0x00018d07 }, /* MAD_0_VFD_3 */
	{ BCHP_MEMC_0_CLIENT_INFO_53, 0x00018d07 }, /* MAD_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_54, 0x00010702 }  /* MAD_0_QM    */
};

static const BVDC_P_AddrDataPair s_Mad0_D2[] = /* Scaled in the MAD (i.e. for PIG) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_49, 0x00035313 }, /* MAD_0_VFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_50, 0x00035313 }, /* MAD_0_VFD_1 */
	{ BCHP_MEMC_0_CLIENT_INFO_51, 0x00035313 }, /* MAD_0_VFD_2 */
	{ BCHP_MEMC_0_CLIENT_INFO_52, 0x00035313 }, /* MAD_0_VFD_3 */
	{ BCHP_MEMC_0_CLIENT_INFO_53, 0x00035313 }, /* MAD_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_54, 0x00018d09 }  /* MAD_0_QM    */
};

static const BVDC_P_AddrDataPair s_Gfx0_G1[] = /* Normal */
{
	{ BCHP_MEMC_0_CLIENT_INFO_40, 0x00008c01 }, /* MFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_29, 0x00007c00 }  /* GFD_0 */
};

static const BVDC_P_AddrDataPair s_Gfx0_G2[] = /* Full screen graphics (JPEG) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_40, 0x00003301 }, /* MFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_29, 0x00005a00 }  /* GFD_0 */
};


/***************************************************************************
 * Reconfigure RTS for selected window path
 *
 * All related HW block are down to change to a new setting (Cx)
 *
 * Blockout for CAP0/VFD0 will have 5 potential settings modes:
 *   Setting C1: PC input RGB (CAP/VFD 10-bit 444, ANR off, MAD off)
 *   Setting C2: 1080p/720p/480p source (CAP/VFD 10-bit 422, MAD off)
 *   Setting C3: 1080i source (CAP/VFD 10-bit 422)
 *   Setting C4: 480i source (CAP/VFD 10-bit 422)
 *   Setting C5: MPEG source Scaled to PIG (CAP/VFD 10-bit 422)
 *
 * Blockout for MFD will have 2 potential settings modes:
 *   Setting M1: Normal
 *   Setting M2: Full screen graphics (JPEG)
 *
 * Blockout for GFD will have 2 potential settings modes:
 *   Setting G1: Normal
 *   Setting G2: Full screen graphics (JPEG)
 *
 * Blockout for ANR will have 3 potential settings modes:
 *   Setting A1: 1080p source
 *   Setting A2 1080i/720p/480p source
 *   Setting A3: 480i source
 *
 * Blockout for MAD0 will have 2 potential settings modes:
 *   Setting D1: Not Scaled in the MAD
 *   Setting D2: Scaled in the MAD (i.e. for PIG)
 */
void BVDC_P_Rts_Reconfig_isr
	( const BVDC_P_WindowContext      *pWindow,
	  const BVDC_P_SourceContext      *pSource,
	  const BAVC_MVD_Field            *pMvdFieldData )
{
	const BVDC_P_AddrDataPair *pCapRts;
	const BVDC_P_AddrDataPair *pMadRts;
	const BVDC_P_AddrDataPair *pAnrRts;
	const BVDC_P_AddrDataPair *pGfxRts;

	BSTD_UNUSED(pMvdFieldData);
	
	/* Need to handle dynamics RTS for: VFD/CAP/MAD/ANR/GFX/MFD */
	if(BVDC_P_WindowId_eComp0_V0 != pWindow->eId)
	{
		return;
	}

	BDBG_MSG(("3553 Dynamics RTS"));

	/* (1) Capture/Feeder RTS */
	if((!BVDC_P_VNET_USED_MAD(pWindow->stVnetMode) &&
	    !BVDC_P_VNET_USED_ANR(pWindow->stVnetMode)) &&
	   (BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	    BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	    BPXL_IS_YCbCr444_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat)))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C1]: 10-bit 444, ANR off, MAD off", pWindow->eId));
		pCapRts = s_Cap0_C1;
	}
	else if((!BVDC_P_VNET_USED_MAD(pWindow->stVnetMode)) &&
	        (BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	         BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat)))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C2]: 10-bit 422, MAD off", pWindow->eId));
		pCapRts = s_Cap0_C2;
	}
	else if((BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	         BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat)) &&
	        (!pSource->stCurInfo.pVdcFmt->bSd))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C3]: 10-bit 422 HD", pWindow->eId));
		pCapRts = s_Cap0_C3;
	}
	else if((BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	         BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat)))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C4]: 10-bit 422 SD", pWindow->eId));
		pCapRts = s_Cap0_C4;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C5]: mpeg src", pWindow->eId));
		pCapRts = s_Cap0_C5;
	}

	/* (2) Anr RTS */
	if((pSource->stCurInfo.pVdcFmt->bHd) &&
	   (pSource->stCurInfo.pVdcFmt->bProgressive) &&
	   (pSource->stCurInfo.pFmtInfo->ulWidth >= BFMT_1080P_WIDTH))
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A1]: 1080p.", pWindow->eId));
		pAnrRts = s_Anr0_A1;
	}
	else if((pSource->stCurInfo.pVdcFmt->bHd) ||
	        (pSource->stCurInfo.pVdcFmt->bEd))
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A2]: ED/HD.", pWindow->eId));
		pAnrRts = s_Anr0_A2;
	}
	else /* Sd */
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A3]: SD.", pWindow->eId));
		pAnrRts = s_Anr0_A3;
	}

	/* (3) Mad RTS */
	if(pWindow->bDstFullScreen)
	{
		BDBG_MSG(("pWindow[%d] mad rts mode[D1]: FullScreen.", pWindow->eId));
		pMadRts = s_Mad0_D1;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] mad rts mode[D2]: PIP/PIG.", pWindow->eId));
		pMadRts = s_Mad0_D2;
	}

	/* (4) Gfx/Mfd!  Application must turn off other windows! */
	if(BVDC_P_SRC_IS_MPEG(pSource->eId) &&
	   hSource->hMpegFeeder->bGfxSrc)
	{
		BDBG_MSG(("pWindow[%d] gfx rts mode[G2]: JPEG Mode.", pWindow->eId));
		pGfxRts = s_Gfx0_G2;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] gfx rts mode[G1]: Normal Mode.", pWindow->eId));
		pGfxRts = s_Gfx0_G1;
	}

	/* Write them out */
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pCapRts,
		sizeof(s_Cap0_C1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pAnrRts,
		sizeof(s_Anr0_A1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pMadRts,
		sizeof(s_Mad0_D1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pGfxRts,
		sizeof(s_Gfx0_G1) / sizeof(BVDC_P_AddrDataPair));

	return;
}

#else /* (BVDC_DYNAMICS_RTS==3563) */
/***************************************************************************
 * RTS Tables
 */
static const BVDC_P_AddrDataPair s_Cap0_C1[] = /* PC input RGB (CAP/VFD 10-bit 444, ANR off, MAD off) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00005f04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00006403 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C2[] = /* 1080p/720p/480p source (CAP/VFD 10-bit 422, MAD off) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00009d04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009703 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C3[] = /* 1080i source (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x00013d04 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x0000be03 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C4[] = /* 480i source (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x0001b704 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009703 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Cap0_C5[] = /* MPEG source Scaled to PIG (CAP/VFD 10-bit 422) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x0001b704 }, /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x00009703 }  /* VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A1[] = /* 1080p source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x0000c507 },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x0000c507 }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A2[] = /* 1080i/720p/480p source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x00017e06 },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x00017e06 }   /* ANR_0_VFD_0 */
};

static const BVDC_P_AddrDataPair s_Anr0_A3[] = /* 480i source */
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x00044c1a },  /* ANR_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x00044c1a }   /* ANR_0_VFD_0 */
};


static const BVDC_P_AddrDataPair s_Mad0_D1[] = /* Not Scaled in the MAD */
{
	{ BCHP_MEMC_0_CLIENT_INFO_49, 0x00018d07 }, /* MAD_0_VFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_50, 0x00018d07 }, /* MAD_0_VFD_1 */
	{ BCHP_MEMC_0_CLIENT_INFO_51, 0x00018d07 }, /* MAD_0_VFD_2 */
	{ BCHP_MEMC_0_CLIENT_INFO_52, 0x00018d07 }, /* MAD_0_VFD_3 */
	{ BCHP_MEMC_0_CLIENT_INFO_53, 0x00018d07 }, /* MAD_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_54, 0x00010702 }  /* MAD_0_QM    */
};

static const BVDC_P_AddrDataPair s_Mad0_D2[] = /* Scaled in the MAD (i.e. for PIG) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_49, 0x00035313 }, /* MAD_0_VFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_50, 0x00035313 }, /* MAD_0_VFD_1 */
	{ BCHP_MEMC_0_CLIENT_INFO_51, 0x00035313 }, /* MAD_0_VFD_2 */
	{ BCHP_MEMC_0_CLIENT_INFO_52, 0x00035313 }, /* MAD_0_VFD_3 */
	{ BCHP_MEMC_0_CLIENT_INFO_53, 0x00035313 }, /* MAD_0_CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_54, 0x00018d09 }  /* MAD_0_QM    */
};

static const BVDC_P_AddrDataPair s_Gfx0_G1[] = /* Normal */
{
	{ BCHP_MEMC_0_CLIENT_INFO_40, 0x0000c505 }, /* MFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_29, 0x00007c00 }  /* GFD_0 */
};

static const BVDC_P_AddrDataPair s_Gfx0_G2[] = /* Full screen graphics (JPEG) */
{
	{ BCHP_MEMC_0_CLIENT_INFO_40, 0x00003305 }, /* MFD_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_29, 0x00005b00 }  /* GFD_0 */
};


/***************************************************************************
 * Reconfigure RTS for selected window path
 *
 * All related HW block are down to change to a new setting (Cx)
 *
 * Blockout for CAP0/VFD0 will have 5 potential settings modes, depending on Main Window:
 *   Setting C1: PC input RGB (CAP/VFD 10-bit 444, ANR off, MAD off)
 *   Setting C2: 1080p/720p/480p source (CAP/VFD 10-bit 422, MAD off)
 *   Setting C3: 1080i source (CAP/VFD 8-bit 422)
 *   Setting C4: 480i source (CAP/VFD 10-bit 422)
 *   Setting C5: MPEG source Scaled to PIG
 *
 * Blockout for ANR0 will have 3 potential settings modes:
 *   Setting A1: 1080p source
 *   Setting A2 1080i/720p/480p source
 *   Setting A3: 480i source
 *
 * Blockout for MAD0 will have 2 potential settings modes:
 *   Setting D1: Not Scaled in the MAD
 *   Setting D2: Scaled in the MAD (i.e. for PIG)
 */
void BVDC_P_Rts_Reconfig_isr
	( const BVDC_P_WindowContext      *pWindow,
	  const BVDC_P_SourceContext      *pSource,
	  const BAVC_MVD_Field            *pMvdFieldData )
{
	const BVDC_P_AddrDataPair *pCapRts;
	const BVDC_P_AddrDataPair *pMadRts;
	const BVDC_P_AddrDataPair *pAnrRts;
	const BVDC_P_AddrDataPair *pGfxRts;

	BSTD_UNUSED(pMvdFieldData);

	/* Need to handle dynamics RTS for: VFD/CAP/MAD/ANR/GFX/MFD */
	if(BVDC_P_WindowId_eComp0_V0 != pWindow->eId)
	{
		return;
	}

	BDBG_MSG(("3563 Dynamics RTS"));

	/* (1) Capture/Feeder RTS */
	if((!BVDC_P_VNET_USED_MAD(pWindow->stVnetMode) &&
	    !BVDC_P_VNET_USED_ANR(pWindow->stVnetMode)) &&
	   (BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	    BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	    BPXL_IS_YCbCr444_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat)))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C1]: 10-bit 444, ANR off, MAD off", pWindow->eId));
		pCapRts = s_Cap0_C1;
	}
	else if((!BVDC_P_VNET_USED_MAD(pWindow->stVnetMode)) &&
	        (BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat) ||
	         BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat)))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C2]: 10-bit 422, MAD off", pWindow->eId));
		pCapRts = s_Cap0_C2;
	}
	else if(BPXL_IS_YCbCr422_FORMAT(pWindow->hBuffer->ePixelFormat))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C3]: 8-bit 422", pWindow->eId));
		pCapRts = s_Cap0_C3;
	}
	else if(BPXL_IS_YCbCr422_10BIT_FORMAT(pWindow->hBuffer->ePixelFormat))
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C4]: 10-bit 422", pWindow->eId));
		pCapRts = s_Cap0_C4;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] cap rts mode[C5]: mpeg src", pWindow->eId));
		pCapRts = s_Cap0_C5;
	}

	/* (2) Anr RTS */
	if((pSource->stCurInfo.pVdcFmt->bHd) &&
	   (pSource->stCurInfo.pVdcFmt->bProgressive) &&
	   (pSource->stCurInfo.pFmtInfo->ulWidth >= BFMT_1080P_WIDTH))
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A1]: 1080p.", pWindow->eId));
		pAnrRts = s_Anr0_A1;
	}
	else if((pSource->stCurInfo.pVdcFmt->bHd) ||
	        (pSource->stCurInfo.pVdcFmt->bEd))
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A2]: ED/HD.", pWindow->eId));
		pAnrRts = s_Anr0_A2;
	}
	else /* Sd */
	{
		BDBG_MSG(("pWindow[%d] anr rts mode[A3]: SD.", pWindow->eId));
		pAnrRts = s_Anr0_A3;
	}

	/* (3) Mad RTS */
	if(pWindow->bDstFullScreen)
	{
		BDBG_MSG(("pWindow[%d] mad rts mode[D1]: FullScreen.", pWindow->eId));
		pMadRts = s_Mad0_D1;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] mad rts mode[D2]: PIP/PIG.", pWindow->eId));
		pMadRts = s_Mad0_D2;
	}

	/* (4) Gfx/Mfd!  Application must turn off other windows! */
	if(BVDC_P_SRC_IS_MPEG(pSource->eId) &&
	   hSource->hMpegFeeder->bGfxSrc)
	{
		BDBG_MSG(("pWindow[%d] gfx rts mode[G2]: JPEG Mode.", pWindow->eId));
		pGfxRts = s_Gfx0_G2;
	}
	else
	{
		BDBG_MSG(("pWindow[%d] gfx rts mode[G1]: Normal Mode.", pWindow->eId));
		pGfxRts = s_Gfx0_G1;
	}

	/* Write them out */
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pCapRts,
		sizeof(s_Cap0_C1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pAnrRts,
		sizeof(s_Anr0_A1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pMadRts,
		sizeof(s_Mad0_D1) / sizeof(BVDC_P_AddrDataPair));
	BVDC_P_Rts_SetClientInfo_isr(pWindow->hCapture->hRegister, pGfxRts,
		sizeof(s_Gfx0_G1) / sizeof(BVDC_P_AddrDataPair));

	return;
}

#endif /* BVDC_DYNAMICS_RTS */

/* TODO: Obsoleted, and removed to improve readability. */
#else /* pre-3563c0 */
/* RTS setting CAP_0/VFD_0: progressive source */
static const BVDC_P_AddrDataPair s_aCap0_ProgIn_RtsTable[] =
{

	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x5f02 },  /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x5901 }   /* VFD_0 */
};

/* RTS setting CAP_0/VFD_0: interlaced source */
static const BVDC_P_AddrDataPair s_aCap0_InterIn_RtsTable[] =
{
	{ BCHP_MEMC_0_CLIENT_INFO_33, 0x8f02 },  /* CAP_0 */
	{ BCHP_MEMC_0_CLIENT_INFO_38, 0x8701 },  /* VFD_0 */
};

/* RTS setting CAP_1/VFD_1: MAD off */
static const BVDC_P_AddrDataPair s_aCap1_MadOff_RtsTable[] =
{
	{ BCHP_MEMC_1_CLIENT_INFO_8,  0x8f03 },  /* CAP_1 */
	{ BCHP_MEMC_1_CLIENT_INFO_10, 0x8701 },  /* VFD_1 */
};

/* RTS setting CAP_1/VFD_1: MAD on, ANR off */
static const BVDC_P_AddrDataPair s_aCap1_MadOnAnrOff_RtsTable[] =
{
	{ BCHP_MEMC_1_CLIENT_INFO_8,  0x13e03 }, /* CAP_1 */
	{ BCHP_MEMC_1_CLIENT_INFO_10, 0x12c01 }, /* VFD_1 */
};

/* RTS setting CAP_1/VFD_1: MAD on, ANR on */
static const BVDC_P_AddrDataPair s_aCap1_MadOnAnrOn_RtsTable[] =
{
	{ BCHP_MEMC_1_CLIENT_INFO_8,  0x35703 }, /* CAP_1 */
	{ BCHP_MEMC_1_CLIENT_INFO_10, 0x3fd01 }, /* VFD_1 */
};

/* RTS setting for MAD_0: normal */
static const BVDC_P_AddrDataPair s_aMad0_Normal_RtsTable[] =
{

	{ BCHP_MEMC_0_CLIENT_INFO_49, 0x18809 }, /* MAD0_FD0 */
	{ BCHP_MEMC_0_CLIENT_INFO_50, 0x18809 }, /* MAD0_FD1 */
	{ BCHP_MEMC_0_CLIENT_INFO_51, 0x18809 }, /* MAD0_FD2 */
	{ BCHP_MEMC_0_CLIENT_INFO_52, 0x18809 }, /* MAD0_FD3 */
	{ BCHP_MEMC_0_CLIENT_INFO_53, 0x18809 }, /* MAD0_CAP */
};

/* RTS setting for MAD_0: MPEG cap or ANR */
static const BVDC_P_AddrDataPair s_aMad0_MpegCapOrAnr_RtsTable[] =
{
	{ BCHP_MEMC_0_CLIENT_INFO_49, 0x31209 }, /* MAD0_FD0 */
	{ BCHP_MEMC_0_CLIENT_INFO_50, 0x31209 }, /* MAD0_FD1 */
	{ BCHP_MEMC_0_CLIENT_INFO_51, 0x31209 }, /* MAD0_FD2 */
	{ BCHP_MEMC_0_CLIENT_INFO_52, 0x31209 }, /* MAD0_FD3 */
	{ BCHP_MEMC_0_CLIENT_INFO_53, 0x31209 }, /* MAD0_CAP */
};

/* RTS setting for ANR_0: Interlaced source: 480i */
static const BVDC_P_AddrDataPair s_aAnr0_InterIn_RtsTable[] =
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x47510 }, /* ANR0_REC_CAP */
	{ BCHP_MEMC_0_CLIENT_INFO_5, 0x47510 }, /* ANR0_IMP_CAP */
	{ BCHP_MEMC_0_CLIENT_INFO_6, 0x47510 }, /* ANR0_IMP0_VFD */
	{ BCHP_MEMC_0_CLIENT_INFO_7, 0x47510 }, /* ANR0_IMP1_VFD */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x47510 }, /* ANR0_REC_VFD */
};

/* RTS setting for ANR_0: Progressive source: 480p */
static const BVDC_P_AddrDataPair s_aAnr0_ProgIn_RtsTable[] =
{
	{ BCHP_MEMC_0_CLIENT_INFO_4, 0x23910 }, /* ANR0_REC_CAP */
	{ BCHP_MEMC_0_CLIENT_INFO_5, 0x23910 }, /* ANR0_IMP_CAP */
	{ BCHP_MEMC_0_CLIENT_INFO_6, 0x23910 }, /* ANR0_IMP0_VFD */
	{ BCHP_MEMC_0_CLIENT_INFO_7, 0x23910 }, /* ANR0_IMP1_VFD */
	{ BCHP_MEMC_0_CLIENT_INFO_8, 0x23910 }, /* ANR0_REC_VFD */
};

/* RTS setting for ANR_1: Interlaced source: 480i */
static const BVDC_P_AddrDataPair s_aAnr1_InterIn_RtsTable[] =
{
	{ BCHP_MEMC_1_CLIENT_INFO_3, 0x47510 }, /* ANR0_REC_CAP */
	{ BCHP_MEMC_1_CLIENT_INFO_4, 0x47510 }, /* ANR0_IMP_CAP */
	{ BCHP_MEMC_1_CLIENT_INFO_5, 0x47510 }, /* ANR0_IMP0_VFD */
	{ BCHP_MEMC_1_CLIENT_INFO_6, 0x47510 }, /* ANR0_IMP1_VFD */
	{ BCHP_MEMC_1_CLIENT_INFO_7, 0x47510 }, /* ANR0_REC_VFD */
};

/* RTS setting for ANR_1: Progressive source: 480p */
static const BVDC_P_AddrDataPair s_aAnr1_ProgIn_RtsTable[] =
{
	{ BCHP_MEMC_1_CLIENT_INFO_3, 0x23910 }, /* ANR0_REC_CAP */
	{ BCHP_MEMC_1_CLIENT_INFO_4, 0x23910 }, /* ANR0_IMP_CAP */
	{ BCHP_MEMC_1_CLIENT_INFO_5, 0x23910 }, /* ANR0_IMP0_VFD */
	{ BCHP_MEMC_1_CLIENT_INFO_6, 0x23910 }, /* ANR0_IMP1_VFD */
	{ BCHP_MEMC_1_CLIENT_INFO_7, 0x23910 }, /* ANR0_REC_VFD */
};


/***************************************************************************
 * Reconfigure RTS for selected window path
 *
 * All related HW block are down to change to a new setting (Ax)
 *
 */
void BVDC_P_Rts_Reconfig_isr
	( const BVDC_P_WindowContext      *pWindow,
	  const BVDC_P_SourceContext      *pSource,
	  const BAVC_MVD_Field            *pMvdFieldData )
{
	BREG_Handle hReg;
	uint32_t ulCount = 0;
	bool bSrcIsProgressive = true;

	BDBG_ENTER(BVDC_P_Rts_Reconfig_isr);

	hReg = pWindow->hCapture->hRegister;

	if(BVDC_P_SRC_IS_MPEG(pSource->eId) && (pMvdFieldData))
	{
		bSrcIsProgressive = (pMvdFieldData->eSourcePolarity == BAVC_Polarity_eFrame);
	}
	else if(BVDC_P_SRC_IS_HDDVI(pSource->eId) || BVDC_P_SRC_IS_ANALOG(pSource->eId))
	{
		bSrcIsProgressive = !pSource->stCurInfo.pFmtInfo->bInterlaced;
	}
	else if(BVDC_P_SRC_IS_ITU656(pSource->eId))
	{
		bSrcIsProgressive = false;
	}

	/* Set RTS based on window id.
	 * BVDC_P_WindowId_eComp2_V0 has same source as BVDC_P_WindowId_eComp0_V0 */
	if(pWindow->eId == BVDC_P_WindowId_eComp0_V0)
	{
		if(!bSrcIsProgressive)
		{
			/* Set RTS for CAP_0 and VFD_0 */
			BDBG_MSG(("Window[%d] RTS setting I for interlace source", pWindow->eId));
			ulCount = sizeof(s_aCap0_InterIn_RtsTable) / sizeof(BVDC_P_AddrDataPair);
			BVDC_P_Rts_SetClientInfo_isr(hReg, s_aCap0_InterIn_RtsTable, ulCount);

			/* Set RTS for ANR_0 */
			if(BVDC_P_VNET_USED_ANR(pWindow->stVnetMode))
			{
				BDBG_MSG(("Window[%d] ANR 0 RTS setting II for interlace source", pWindow->eId));
				ulCount = sizeof(s_aAnr0_InterIn_RtsTable) / sizeof(BVDC_P_AddrDataPair);
				BVDC_P_Rts_SetClientInfo_isr(hReg, s_aAnr0_InterIn_RtsTable, ulCount);
			}
		}
		else
		{
			/* Set RTS for CAP_0 and VFD_0 */
			BDBG_MSG(("Window[%d] RTS setting II for progressive source", pWindow->eId));
			ulCount = sizeof(s_aCap0_ProgIn_RtsTable) / sizeof(BVDC_P_AddrDataPair);
			BVDC_P_Rts_SetClientInfo_isr(hReg, s_aCap0_ProgIn_RtsTable, ulCount);

			/* Set RTS for ANR_0 */
			if(BVDC_P_VNET_USED_ANR(pWindow->stVnetMode))
			{
				BDBG_MSG(("Window[%d] ANR 0 RTS setting II for progressive source", pWindow->eId));
				ulCount = sizeof(s_aAnr0_ProgIn_RtsTable) / sizeof(BVDC_P_AddrDataPair);
				BVDC_P_Rts_SetClientInfo_isr(hReg, s_aAnr0_ProgIn_RtsTable, ulCount);
			}
		}

		/* Set RTS for MAD_0 */
		if((BVDC_P_SRC_IS_MPEG(pSource->eId) &&
		    BVDC_P_VNET_USED_CAPTURE(pWindow->stVnetMode)) ||
		    BVDC_P_VNET_USED_ANR(pWindow->stVnetMode))
		{
			BDBG_MSG(("Window[%d] MAD 0 RTS setting I for MPEG CAP or ANR source", pWindow->eId));
			ulCount = sizeof(s_aMad0_MpegCapOrAnr_RtsTable) / sizeof(BVDC_P_AddrDataPair);
			BVDC_P_Rts_SetClientInfo_isr(hReg, s_aMad0_MpegCapOrAnr_RtsTable, ulCount);
		}
		else
		{
			/* normal */
			BDBG_MSG(("Window[%d] MAD 0 RTS setting II for normal source", pWindow->eId));
			ulCount = sizeof(s_aMad0_Normal_RtsTable) / sizeof(BVDC_P_AddrDataPair);
			BVDC_P_Rts_SetClientInfo_isr(hReg, s_aMad0_Normal_RtsTable, ulCount);

		}
		/* Set RTS for ANR_0 */
	}
	else if(pWindow->eId == BVDC_P_WindowId_eComp0_V1)
	{
		/* Set RTS for CAP_1 and VFD_1 */
		if(BVDC_P_VNET_USED_MAD(pWindow->stVnetMode))
		{
			if(BVDC_P_VNET_USED_ANR(pWindow->stVnetMode))
			{
				BDBG_MSG(("Window[%d] RTS setting III for interlace SD source", pWindow->eId));
				ulCount = sizeof(s_aCap1_MadOnAnrOn_RtsTable) / sizeof(BVDC_P_AddrDataPair);
				BVDC_P_Rts_SetClientInfo_isr(hReg, s_aCap1_MadOnAnrOn_RtsTable, ulCount);
			}
			else
			{
				BDBG_MSG(("Window[%d] RTS setting II for interlace Non-SD source", pWindow->eId));
				ulCount = sizeof(s_aCap1_MadOnAnrOff_RtsTable) / sizeof(BVDC_P_AddrDataPair);
				BVDC_P_Rts_SetClientInfo_isr(hReg, s_aCap1_MadOnAnrOff_RtsTable, ulCount);
			}
		}
		else
		{
			BDBG_MSG(("Window[%d] RTS setting I for progressive source", pWindow->eId));
			ulCount = sizeof(s_aCap1_MadOff_RtsTable) / sizeof(BVDC_P_AddrDataPair);
			BVDC_P_Rts_SetClientInfo_isr(hReg, s_aCap1_MadOff_RtsTable, ulCount);
		}

		/* Set RTS for ANR_1 */
		if(BVDC_P_VNET_USED_ANR(pWindow->stVnetMode))
		{
			if(!bSrcIsProgressive)
			{
				BDBG_MSG(("Window[%d] ANR 1 RTS setting II for interlace source", pWindow->eId));
				ulCount = sizeof(s_aAnr1_InterIn_RtsTable) / sizeof(BVDC_P_AddrDataPair);
				BVDC_P_Rts_SetClientInfo_isr(hReg, s_aAnr1_InterIn_RtsTable, ulCount);
			}
			else
			{
				BDBG_MSG(("Window[%d] ANR 1 RTS setting II for progressive source", pWindow->eId));
				ulCount = sizeof(s_aAnr1_ProgIn_RtsTable) / sizeof(BVDC_P_AddrDataPair);
				BVDC_P_Rts_SetClientInfo_isr(hReg, s_aAnr1_ProgIn_RtsTable, ulCount);
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Rts_Reconfig_isr);
	return;
}
#endif

#else /* BVDC_P_SUPPORT_DYNAMIC_RTS */
void BVDC_P_Rts_Reconfig_isr
	( const BVDC_P_WindowContext      *pWindow,
	  const BVDC_P_SourceContext      *pSource,
	  const BAVC_MVD_Field            *pMvdFieldData )
{
	BSTD_UNUSED(pWindow);
	BSTD_UNUSED(pSource);
	BSTD_UNUSED(pMvdFieldData);
	return;
}

#endif

/* End of file. */
