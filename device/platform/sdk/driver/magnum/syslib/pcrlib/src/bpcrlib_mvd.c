/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib_mvd.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 12/4/06 1:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7038/bpcrlib_mvd.c $
 * 
 * Hydra_Software_Devel/8   12/4/06 1:39p erickson
 * PR26295: only call BXPT_PcrOffset_RegenOffset_isr on request_stc, not
 * pts_error. this requires a new param to UpdateStc.
 * 
 * Hydra_Software_Devel/7   8/29/05 6:24p vsilyaev
 * PR 16685,PR15967: Added use of UpdateStc function.
 * 
 * Hydra_Software_Devel/6   8/16/05 3:53p vsilyaev
 * PR 15967: Added debug traces.
 * 
 * Hydra_Software_Devel/5   8/9/05 2:31p vsilyaev
 * PR 15967: Use both aux transport and decoder to access STC.
 * 
 * Hydra_Software_Devel/4   8/5/05 3:07p vsilyaev
 * PR 15967: Fixed compile time warnings
 * 
 * Hydra_Software_Devel/3   6/30/05 9:21a erickson
 * PR15697: changed to BMVD_GetPTS_isr to avoid deadlok
 * 
 * Hydra_Software_Devel/2   6/28/05 1:16p vsilyaev
 * PR 15697: Improved interface to update STC.
 * 
 * Hydra_Software_Devel/1   6/24/05 3:50p vsilyaev
 * PR 15967: 7038 implementation of the audio/video interface.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bpcrlib.h"
#include "bpcrlib_mvd.h"
#include "bmvd.h"
#include "bxpt_directv_pcr.h"


BDBG_MODULE(pcrlib);

static BERR_Code 
BPCRlib_Video_GetStc_MVD_isr(void *trp, void *dec, uint32_t *stc)
{
	BSTD_UNUSED(trp);

	BDBG_MSG(("VID_MVD:GetStc %#x %#x", (unsigned)trp, (unsigned)dec));
	return BMVD_GetCurrentLocalSTC(dec, stc);
}

static BERR_Code 
BPCRlib_Video_GetPts_MVD_isr(void *dec, BAVC_PTSInfo *pts)
{
	BMVD_PTSInfo			PTSInfo;
	BERR_Code rc;

	BDBG_MSG(("VID_MVD:GetPts %#x", (unsigned)dec));
	rc = BMVD_GetPTS_isr(dec, &PTSInfo);
	pts->ui32CurrentPTS = PTSInfo.ui32RunningPTS;
	pts->ePTSType = PTSInfo.ePTSType;
	
	return rc;
}

BERR_Code 
BPCRlib_Video_GetCdbLevel_MVD_isr(void *dec, unsigned *level)
{
	return BMVD_GetCDBOccupancy(dec, (void *)level);
}

static BERR_Code 
BPCRlib_Video_SetStc_MVD_isr(void *trp, void *dec, bool dss, uint32_t stc)
{
	BERR_Code rc;

	BSTD_UNUSED(dec);

	BDBG_MSG(("VID::SetStc %#x", (unsigned)trp, (unsigned)dec));
	if (dss) {
		BDBG_MSG(("updating XPT: DSS stc %#x", stc));
		rc = BXPT_PCR_DirecTv_SendPcr_isr(trp, stc);
		if (rc!=BERR_SUCCESS) { BDBG_ERR(("BXPT_PCR_DirecTv_SendPcr_isr returned error %#x, ignored", rc)); }
	} else {
		BDBG_MSG(("updating XPT: stc %#x", stc));
		rc = BXPT_PCR_SendPcr_isr(trp, stc, 0);
		if (rc!=BERR_SUCCESS) { BDBG_ERR(("BXPT_PCR_SendPcr_isr returned error %#x, ignored", rc)); }
	}
	return rc;
}
	
static BERR_Code 
BPCRlib_Video_UpdateStc_MVD_isr(void *trp, bool is_request_stc)
{
	BSTD_UNUSED(is_request_stc);
	BDBG_MSG(("VID::UpdateStc %#x", (unsigned)trp));
	BXPT_PCR_RefreshPcrPid_isr(trp);
	return BERR_SUCCESS;
}

const BPCRlib_StcDecIface BPCRlib_Video_Mvd = {
	BPCRlib_Video_GetPts_MVD_isr,
	BPCRlib_Video_GetStc_MVD_isr,
	BPCRlib_Video_GetCdbLevel_MVD_isr,
	BPCRlib_Video_SetStc_MVD_isr,
	BPCRlib_Video_UpdateStc_MVD_isr,
	false
};

