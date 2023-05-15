/***************************************************************************
 *     Copyright (c) 2005-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib_rap.c $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 3/21/08 11:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7038/bpcrlib_rap.c $
 * 
 * Hydra_Software_Devel/20   3/21/08 11:25a katrep
 * PR40812:Add support for RAP PI 3.0 new FW arch
 * 
 * Hydra_Software_Devel/19   10/4/07 11:59a gmohile
 * PR 25109 : use _isr function
 * 
 * Hydra_Software_Devel/18   8/23/07 11:28a katrep
 * PR27643: Use BRAP_SetStcValidFlag_isr now available wit phase 0.5
 * raptor
 * 
 * Hydra_Software_Devel/17   7/20/07 12:30p katrep
 * PR27643: Added support for RAP PI 2.0 required for 7405
 * 
 * 
 * Hydra_Software_Devel/16   2/28/07 9:02p jgarrett
 * PR 25900: Updating call to BXPT_PcrOffset_GetStc_isr
 * 
 * Hydra_Software_Devel/15   12/5/06 1:57p ahulse
 * PR26295: Suppress warning for unused bool when building for 7411
 * 
 * Hydra_Software_Devel/14   12/4/06 1:39p erickson
 * PR26295: only call BXPT_PcrOffset_RegenOffset_isr on request_stc, not
 * pts_error. this requires a new param to UpdateStc.
 * 
 * Hydra_Software_Devel/13   11/22/06 11:33a erickson
 * PR26126: call BXPT_PcrOffset_RegenOffset_isr when live-mode PTS
 * interrupts are received. this ensures the decoders will see a valid
 * PCR OFFSET when system is configured with non-zero
 * PCR_OFFSET.OFFSET_THRESHOLD.
 * 
 * Hydra_Software_Devel/12   10/2/06 4:13p vsilyaev
 * PR 23826: Fixed comparison on module arithmetic
 * 
 * Hydra_Software_Devel/11   9/14/06 3:36p jgarrett
 * PR 23982: Removing release mode compiler warning.
 * 
 * Hydra_Software_Devel/10   8/9/06 1:12p erickson
 * PR20199: don't reset the STC if already very close. small adjustments
 * cause raptor glitches because it's trying to maintain a tight TSM
 * threshold window.
 * 
 * Hydra_Software_Devel/9   2/3/06 3:50p vsilyaev
 * PR 18642: Update PCRlibrary to support playback of constant bitrate
 * streams
 * 
 * Hydra_Software_Devel/PR18642/1   1/19/06 6:16p vsilyaev
 * PR18642: Use PTS offset to control buffer depth in the decoders
 * 
 * Hydra_Software_Devel/8   11/11/05 12:57p erickson
 * PR17108: calculate correct STC for 7401
 * 
 * Hydra_Software_Devel/7   10/4/05 9:11a erickson
 * PR17108: modified for XPT api changes
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
 * Hydra_Software_Devel/1   7/21/05 12:36p vsilyaev
 * PR 15967: Added raptor STC interface
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bpcrlib.h"
#include "bpcrlib_rap.h"
#ifndef BCHP_7411_VER
#include "bxpt_pcr_offset.h"
#endif

#if BRAP_VER >= 2
#define BRAP_DSPCHN_GetCurrentPTS_isr(x,Y) BRAP_GetCurrentPTS_isr(x,Y)
#define BRAP_DSPCHN_SetStcValidFlag_isr(x) BRAP_SetStcValidFlag_isr(x) 
#endif

BDBG_MODULE(pcrlib);

static BERR_Code 
BPCRlib_Audio_GetStc_Rap_isr(void *trp, void *dec, uint32_t *stc)
{
    BSTD_UNUSED(dec);
    BDBG_MSG(("RAP:GetStc %#x %#x", (unsigned)trp, (unsigned)dec));
#ifdef BCHP_7411_VER
    return BARC_GetSTC_isr(trp, stc);
#else
    *stc = BXPT_PcrOffset_GetStc_isr(trp) + BXPT_PcrOffset_GetOffset_isr(trp);
    return BERR_SUCCESS;
#endif
}

static BERR_Code 
BPCRlib_Audio_GetPts_Rap_isr(void *dec, BAVC_PTSInfo *pts)
{
    BRAP_DSPCHN_PtsInfo PTSInfo;
    BERR_Code rc;
    const BPCRlib_Rap_Decoder *rap=dec;

    BDBG_MSG(("RAP:GetPts %#x", (unsigned)dec));
    rc = BRAP_DSPCHN_GetCurrentPTS_isr(rap->dec, &PTSInfo);
    pts->ui32CurrentPTS = PTSInfo.ui32RunningPts;
    pts->ePTSType = PTSInfo.ePtsType;
    
    return rc;
}

static BERR_Code 
BPCRlib_Audio_GetCdbLevel_Rap_isr(void *dec, unsigned *level)
{
    const BPCRlib_Rap_Decoder *rap=dec;
#ifdef BCHP_7411_VER
    return BRAP_TRANS_GetCdbOccupancy_isr(rap->trans,  level);
#else
    BXPT_Rave_BufferInfo buffer_info;
    BERR_Code rc;

    BDBG_ASSERT(rap->rave);
    rc = BXPT_Rave_GetBufferInfo_isr(rap->rave, &buffer_info);
    if (rc==BERR_SUCCESS) {
        *level = buffer_info.CdbDepth;
    }
    return rc;
#endif
}


static BERR_Code 
BPCRlib_Audio_SetStc_Rap_isr(void *trp, void *dec, bool dss, uint32_t stc)
{
    BERR_Code  rc = BERR_SUCCESS;
    const BPCRlib_Rap_Decoder *rap=dec;
    BSTD_UNUSED(dss);

    BDBG_MSG(("RAP:SetStc %#x %#x", (unsigned)trp, (unsigned)dec));

#ifdef BCHP_7411_VER
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
    
#endif
    if (rc==BERR_SUCCESS) {
        rc = BRAP_DSPCHN_SetStcValidFlag_isr(rap->dec);
    }
    return rc;
}

static BERR_Code 
BPCRlib_Audio_UpdateStc_Rap_isr(void *trp, bool is_request_stc)
{
    BDBG_MSG(("RAP::UpdateStc %#x", (unsigned)trp));
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

const BPCRlib_StcDecIface BPCRlib_Audio_Rap = {
    BPCRlib_Audio_GetPts_Rap_isr,
    BPCRlib_Audio_GetStc_Rap_isr,
    BPCRlib_Audio_GetCdbLevel_Rap_isr,
    BPCRlib_Audio_SetStc_Rap_isr, 
    BPCRlib_Audio_UpdateStc_Rap_isr,  /* no update STC */
    true
};

