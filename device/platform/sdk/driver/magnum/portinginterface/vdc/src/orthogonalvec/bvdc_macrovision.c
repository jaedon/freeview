/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_macrovision.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 6/29/12 12:29p $
 *
 * Module Description:
 *   Main module for Macrovision functions.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/orthogonalvec/bvdc_macrovision.c $
 * 
 * Hydra_Software_Devel/5   6/29/12 12:29p darnstein
 * HW7552-167: Support the VAMP on/off bit in CPC array for custom
 * Macrovision.
 * 
 * Hydra_Software_Devel/4   3/8/12 12:05p yuxiaz
 * SW7425-2546: Fixed warning msg.
 * 
 * Hydra_Software_Devel/3   5/7/10 7:22p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 * 
 * Hydra_Software_Devel/2   3/17/09 11:57a darnstein
 * PR51527: Late merge of Macrovision software.
 * 
 * Hydra_Software_Devel/1   3/16/09 10:57p tdo
 * PR45785, PR45789: Merge from MCVP branch
 * 
 * Hydra_Software_Devel/7420_mcvp/4   3/13/09 5:52p darnstein
 * PR51527: Macrovision working now, but it reloads microcode for each
 * change. So more work remains.
 * 
 * Hydra_Software_Devel/7420_mcvp/3   3/11/09 3:31p darnstein
 * PR51527: Disable Macrovision API functions for now. They cause bad
 * video timing.
 * 
 * Hydra_Software_Devel/7420_mcvp/2   3/10/09 12:30p darnstein
 * PR51527: Macrovision API functions set bTiming dirty bit.
 * 
 * Hydra_Software_Devel/7420_mcvp/1   3/9/09 2:51p darnstein
 * PR51527: Need a dedicated version for 7420.
 * 
 * Hydra_Software_Devel/21   7/17/08 3:34p darnstein
 * PR44600: Initialize NO bits to false when PAL, and customized
 * Macrovision.
 * 
 * Hydra_Software_Devel/20   2/21/08 3:42p darnstein
 * PR39783: Improve selection logic for Macrovision features (mostly
 * cosmetic). Fix starting addresses for VEC microcode for Macrovision /
 * PAL_M.
 * 
 * Hydra_Software_Devel/19   10/30/07 7:20p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/18   10/10/07 5:19p darnstein
 * PR35195: For PAL test01 and test02 Macrovision, restore the colorstripe
 * waveform. Although there is no CS waveform for type[012], there is CS
 * waveform for test0[12].
 *
 * Hydra_Software_Devel/17   10/5/07 5:45p darnstein
 * PR35195: these are the same changes I made on the dedicated 7401
 * branch. Three new Macrovision types for PAL.
 *
 * Hydra_Software_Devel/16   4/4/06 1:49p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/15   9/21/05 6:24p albertl
 * PR16459:  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 * Hydra_Software_Devel/14   9/12/05 2:23p hongtaoz
 * PR16822, PR15749: updated PAL microcode; fixed Macrovision
 * certification control process;
 *
 * Hydra_Software_Devel/13   8/18/05 1:16p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/15/05 12:07p hongtaoz
 * PR15749:  added Macrovision support for new test process, i.e. toggling
 * of certain CPC control bits;
 *
 * Hydra_Software_Devel/12   8/14/05 1:16p hongtaoz
 * PR15749: added Macrovision support for new test process, i.e. toggling
 * of certain CPC control bits;
 *
 * Hydra_Software_Devel/11   3/17/05 6:39p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/10   11/3/04 5:00p hongtaoz
 * PR13164: CPC/CPS tables are run-time variable type; also removed
 * private function from the public header file;
 *
 * Hydra_Software_Devel/9   7/7/04 2:36p hongtaoz
 * PR7726: added Macrovision type translation support for the run-time
 * CPC/CPS tables loading;
 *
 * Hydra_Software_Devel/8   7/6/04 8:38a pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.
 ***************************************************************************/

#include "bstd.h"          /* standard types */
#include "bvdc.h"
#include "bvdc_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_macrovision.h"


BDBG_MODULE(BVDC);


/*************************************************************************
 *	BVDC_Display_SetMacrovisionType
 *************************************************************************/
BERR_Code BVDC_Display_SetMacrovisionType
(
	BVDC_Display_Handle              hDisplay,
	BVDC_MacrovisionType             eMacrovisionType
)
{
	bool bFeature;

	BDBG_ENTER(("BVDC_Display_SetMacrovisionType"));

	if ((!hDisplay) || (eMacrovisionType > BVDC_MacrovisionType_eTest02))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BCHP_GetFeature(hDisplay->hVdc->hChip, BCHP_Feature_eMacrovisionCapable, (void *)&bFeature);

	/* if the chip is not macrovision capable, cannot set macrovision protection */
	if((!bFeature) && (eMacrovisionType != BVDC_MacrovisionType_eNoProtection))
	{
		BDBG_ERR(("This chip doesn't support Macrovision!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Bypass path does not support macrovision */
	if (hDisplay->bIsBypass)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_PATH);
	}

	/* Check for valid Macrovision support */
	if ((eMacrovisionType != BVDC_MacrovisionType_eNoProtection) &&
		!VIDEO_FORMAT_SUPPORTS_MACROVISION(hDisplay->stNewInfo.pFmtInfo->eVideoFmt))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
	}

	if (VIDEO_FORMAT_IS_PROGRESSIVE(hDisplay->stNewInfo.pFmtInfo->eVideoFmt) &&
	    ((eMacrovisionType == BVDC_MacrovisionType_eTest01) ||
		 (eMacrovisionType == BVDC_MacrovisionType_eTest02)))
	{
		BDBG_ERR(("Current format doesn't support this Macrovision Test Configuration!"));
		return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
	}

	/* Update new macrovision type */
	hDisplay->stNewInfo.eMacrovisionType = eMacrovisionType;

	/* sort out N0 control bits */
	hDisplay->stNewInfo.stN0Bits.bBp    = false;
	hDisplay->stNewInfo.stN0Bits.bCycl  = false;
	hDisplay->stNewInfo.stN0Bits.bHamp  = false;
	hDisplay->stNewInfo.stN0Bits.bVamp  = false;
	hDisplay->stNewInfo.stN0Bits.bPsAgc = false;
	hDisplay->stNewInfo.stN0Bits.bCs    = false;
	hDisplay->stNewInfo.stN0Bits.bRgb   =
		(eMacrovisionType >= BVDC_MacrovisionType_eType1_Rgb);

	if (VIDEO_FORMAT_IS_625_LINES(hDisplay->stNewInfo.pFmtInfo->eVideoFmt))
	{
		/* SECAM, and normal PAL case */
		if ((eMacrovisionType != BVDC_MacrovisionType_eNoProtection) &&
		    (eMacrovisionType != BVDC_MacrovisionType_eCustomized  )    )
		{
			hDisplay->stNewInfo.stN0Bits.bBp    = true;
			hDisplay->stNewInfo.stN0Bits.bCycl  = true;
			hDisplay->stNewInfo.stN0Bits.bHamp  = true;
			hDisplay->stNewInfo.stN0Bits.bPsAgc = true;
			BDBG_MSG(("Turn on Back Porch, CYCL, Hamp, PS/AGC"));
			if ((eMacrovisionType == BVDC_MacrovisionType_eTest01) ||
			    (eMacrovisionType == BVDC_MacrovisionType_eTest02)  )
			{
				hDisplay->stNewInfo.stN0Bits.bCs = true;
				BDBG_MSG(("Turn on CS"));
			}
		}
	}
	else if (VIDEO_FORMAT_IS_525_LINES(hDisplay->stNewInfo.pFmtInfo->eVideoFmt))
	{
		/* NTSC case and PAL_M cases */
		if((eMacrovisionType == BVDC_MacrovisionType_eType1) ||
		   (eMacrovisionType == BVDC_MacrovisionType_eType1_Rgb))
		{
			hDisplay->stNewInfo.stN0Bits.bBp    = true;
			hDisplay->stNewInfo.stN0Bits.bCycl  = true;
			hDisplay->stNewInfo.stN0Bits.bHamp  = true;
			hDisplay->stNewInfo.stN0Bits.bPsAgc = true;
			BDBG_MSG(("Turn on Back Porch, CYCL, Hamp, PS/AGC"));
		}
		else if((eMacrovisionType == BVDC_MacrovisionType_eType2) ||
				(eMacrovisionType == BVDC_MacrovisionType_eType3) ||
				(eMacrovisionType >= BVDC_MacrovisionType_eType2_Rgb))
		{
			hDisplay->stNewInfo.stN0Bits.bBp    = true;
			hDisplay->stNewInfo.stN0Bits.bCycl  = true;
			hDisplay->stNewInfo.stN0Bits.bHamp  = true;
			hDisplay->stNewInfo.stN0Bits.bPsAgc = true;
			hDisplay->stNewInfo.stN0Bits.bCs    = true;
			BDBG_MSG(("Turn on Back Porch, CYCL, Hamp, PS/AGC, Color-Stripes"));
		}
	}
	else
	{
		/* 480P, 576P case */
		hDisplay->stNewInfo.stN0Bits.bBp    = true;
		hDisplay->stNewInfo.stN0Bits.bCycl  = true;
		hDisplay->stNewInfo.stN0Bits.bHamp  = true;
		hDisplay->stNewInfo.stN0Bits.bPsAgc = true;
		BDBG_MSG(("Turn on Back Porch, CYCL, Hamp, PS/AGC"));
	}

	/* Activate the VEC timing event handler. This will do the real work. */
	if ((hDisplay->stNewInfo.eMacrovisionType >= BVDC_MacrovisionType_eTest01) 
		||
		(hDisplay->stCurInfo.eMacrovisionType >= BVDC_MacrovisionType_eTest01))
	{
		hDisplay->stNewInfo.stDirty.stBits.bTiming     = BVDC_P_DIRTY;
	}
	else
	{
		hDisplay->stNewInfo.stDirty.stBits.bSoftTiming = BVDC_P_DIRTY;
	}
	hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;

	BDBG_LEAVE(("BVDC_Display_SetMacrovisionType"));
	return BERR_SUCCESS;
}

/*************************************************************************
 *	BVDC_Display_GetMacrovisionType
 *************************************************************************/
BERR_Code BVDC_Display_GetMacrovisionType
(
	const BVDC_Display_Handle        hDisplay,
	BVDC_MacrovisionType            *peMacrovisionType
)
{
	BDBG_ENTER(("BVDC_Display_GetMacrovisionType"));

	if ((!hDisplay) || (!peMacrovisionType))
	{
		BDBG_ERR(("Invalid parameter"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Bypass path does not support macrovision */
	if (hDisplay->bIsBypass)
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_PATH);
	}

	*peMacrovisionType =
		hDisplay->stCurInfo.eMacrovisionType;

	BDBG_LEAVE(("BVDC_Display_GetMacrovisionType"));
	return BERR_SUCCESS;
}

/* End of File */
