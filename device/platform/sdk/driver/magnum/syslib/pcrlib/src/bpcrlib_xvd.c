/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib_xvd.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 10/5/07 9:47a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7118/bpcrlib_xvd.c $
 * 
 * Hydra_Software_Devel/23   10/5/07 9:47a mward
 * PR25109 : use _isr function, as for
 * BPCRlib_Audio_GetCdbLevel_Rap_isr().
 * 
 * Hydra_Software_Devel/22   6/7/07 7:05p bandrews
 * PR31945: fixed
 * 
 * Hydra_Software_Devel/21   12/4/06 1:39p erickson
 * PR26295: only call BXPT_PcrOffset_RegenOffset_isr on request_stc, not
 * pts_error. this requires a new param to UpdateStc.
 * 
 * Hydra_Software_Devel/20   11/22/06 11:33a erickson
 * PR26126: call BXPT_PcrOffset_RegenOffset_isr when live-mode PTS
 * interrupts are received. this ensures the decoders will see a valid
 * PCR OFFSET when system is configured with non-zero
 * PCR_OFFSET.OFFSET_THRESHOLD.
 * 
 * Hydra_Software_Devel/19   10/2/06 4:13p vsilyaev
 * PR 23826: Fixed comparison on module arithmetic
 * 
 * Hydra_Software_Devel/18   9/14/06 3:36p jgarrett
 * PR 23982: Removing release mode compiler warning.
 * 
 * Hydra_Software_Devel/17   8/9/06 1:12p erickson
 * PR20199: don't reset the STC if already very close. small adjustments
 * cause raptor glitches because it's trying to maintain a tight TSM
 * threshold window.
 * 
 * Hydra_Software_Devel/16   7/21/06 9:08a dlwin
 * PR 14429: Fix compiler warning.
 * 
 * Hydra_Software_Devel/15   5/10/06 4:47p davidp
 * PR20566: BXVD_SetSTCInvalidFlag changed to BXVD_SetSTCInvalidFlag_isr
 * 
 * Hydra_Software_Devel/14   4/11/06 2:32p erickson
 * PR20399: moved TODO
 * 
 * Hydra_Software_Devel/13   4/4/06 10:07p erickson
 * PR20566: BXVD_SetSTCInvalidFlag is now called after pcrlib sets STC
 * 
 * Hydra_Software_Devel/12   2/9/06 12:30p erickson
 * PR19445: convert BXPT_PcrOffset calls to _isr flavors
 * 
 * Hydra_Software_Devel/11   2/3/06 3:50p vsilyaev
 * PR 18642: Update PCRlibrary to support playback of constant bitrate
 * streams
 * 
 * Hydra_Software_Devel/10   11/11/05 3:28p erickson
 * PR17108: fix for 97398
 * 
 * Hydra_Software_Devel/9   11/11/05 12:57p erickson
 * PR17108: calculate correct STC for 7401
 * 
 * Hydra_Software_Devel/8   10/4/05 9:11a erickson
 * PR17108: modified for XPT api changes
 * 
 * Hydra_Software_Devel/7   9/28/05 5:37p erickson
 * PR17108: don't call BXVD_GetSTC yet
 * 
 * Hydra_Software_Devel/6   9/23/05 12:51p erickson
 * PR17108: initial 97401 checkin
 * 
 * Hydra_Software_Devel/5   8/29/05 6:24p vsilyaev
 * PR 16685,PR15967: Added use of UpdateStc function.
 * 
 * Hydra_Software_Devel/4   8/16/05 3:53p vsilyaev
 * PR 15967: Added debug traces.
 * 
 * Hydra_Software_Devel/3   8/9/05 2:32p vsilyaev
 * PR 15967: Use both aux transport and decoder to access STC.
 * 
 * Hydra_Software_Devel/2   8/5/05 3:08p vsilyaev
 * PR 15967: Use _isr versions of appropriate functions
 * 
 * Hydra_Software_Devel/1   6/24/05 3:50p vsilyaev
 * PR 15967: 7038 implementation of the audio/video interface.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bpcrlib.h"
#include "bpcrlib_xvd.h"
#include "bxvd.h"
#ifndef BCHP_7411_VER
#include "bxpt_pcr_offset.h"
#endif

BDBG_MODULE(pcrlib);

static BERR_Code 
BPCRlib_Video_GetStc_XVD_isr(void *trp, void *dec, uint32_t *stc)
{
	BSTD_UNUSED(dec);
	BDBG_MSG(("VID_XVD:GetStc %#x %#x", (unsigned)trp, (unsigned)dec));
#ifdef BCHP_7411_VER
	return BARC_GetSTC_isr(trp, stc);
#else
	*stc = BXPT_PcrOffset_GetStc_isr(trp) + BXPT_PcrOffset_GetOffset_isr(trp);
	return BERR_SUCCESS;
#endif
}

static BERR_Code 
BPCRlib_Video_GetPts_XVD_isr(void *dec, BAVC_PTSInfo *pts)
{
	BXVD_PTSInfo			PTSInfo;
	BERR_Code rc;
	const BPCRlib_Xvd_Decoder *xvd=dec;

	BDBG_MSG(("VID_XVD:GetPts %#x", (unsigned)dec));
	rc = BXVD_GetPTS_isr(xvd->dec, &PTSInfo);
#ifdef BCHP_7411_VER
	pts->ui32CurrentPTS = PTSInfo.ulCurrentPTS;
#else
	pts->ui32CurrentPTS = PTSInfo.ui32RunningPTS;
#endif
	pts->ePTSType = PTSInfo.ePTSType;
	
	return rc;
}

BERR_Code 
BPCRlib_Video_GetCdbLevel_XVD_isr(void *dec, unsigned *level)
{
	const BPCRlib_Xvd_Decoder *xvd=dec;
#ifdef BCHP_7411_VER
	unsigned size;
	return BXVD_GetCPBInfo_isr(xvd->dec, &size, level);
#else
	BXPT_Rave_BufferInfo buffer_info;
	BERR_Code rc;

	BDBG_ASSERT(xvd->rave);
	rc = BXPT_Rave_GetBufferInfo_isr(xvd->rave, &buffer_info);
	if (rc==BERR_SUCCESS) {
		*level = buffer_info.CdbDepth;
	}
	return rc;
#endif
}

static BERR_Code 
BPCRlib_Video_SetStc_XVD_isr(void *trp, void *dec, bool dss,uint32_t stc)
{
	const BPCRlib_Xvd_Decoder *xvd=dec;
	BERR_Code rc = BERR_SUCCESS;
	BSTD_UNUSED(dss);
	BSTD_UNUSED(xvd);

	BDBG_MSG(("VID_XVD:SetStc %#x %#x", (unsigned)trp, (unsigned)dec));
#ifdef BCHP_7411_VER
	BSTD_UNUSED(dec);
	BSTD_UNUSED(xvd);
	rc = BARC_SetSTC_isr(trp, stc);
#else
	{
		uint32_t current_stc;
		int32_t diff;
	
		current_stc = BXPT_PcrOffset_GetStc_isr(trp);
		diff = (int32_t)current_stc - (int32_t)stc;
		/* If the STC is already very close, there's no point in resetting it. This	
		prevents PTS Errors from raptor which tries to follow a tight TSM threshold. */
		if (diff > 100 || diff < -100) {
			/* assume that PCR_OFFSET offset was set to 0 by host before starting PVR. */
			rc = BXPT_PcrOffset_SetStc_isr(trp, stc);
		}
	}
	
	/* notify decoders that STC is now valid */
	if (rc==BERR_SUCCESS) {
		rc = BXVD_SetSTCInvalidFlag_isr(xvd->dec, false);
	}
#endif

	return rc;
}

static BERR_Code 
BPCRlib_Video_UpdateStc_XVD_isr(void *trp, bool is_request_stc)
{
	BDBG_MSG(("XVD::UpdateStc %#x", (unsigned)trp));
#ifdef BCHP_7411_VER
	BSTD_UNUSED(is_request_stc);
	return BARC_UpdateSTC_isr(trp);
#else
	if (is_request_stc) {
		/* If PCR_OFFSET block has non-zero OFFSET_THRESHOLD, then it needs
		to be forced to regenerate an offset message to RAVE. Otherwise the decoder
		may lose a pcr_offset_valid message. */
		BXPT_PcrOffset_RegenOffset_isr(trp);
	}
	return 0;
#endif
}


const BPCRlib_StcDecIface BPCRlib_Video_Xvd = {
	BPCRlib_Video_GetPts_XVD_isr,
	BPCRlib_Video_GetStc_XVD_isr,
	BPCRlib_Video_GetCdbLevel_XVD_isr,
	BPCRlib_Video_SetStc_XVD_isr, 
	BPCRlib_Video_UpdateStc_XVD_isr, 
	true
};

