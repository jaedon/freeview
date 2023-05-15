/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_decoder_trick.c $
 * $brcm_Revision: 49 $
 * $brcm_Date: 10/10/12 10:03a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder_trick.c $
 * 
 * 49   10/10/12 10:03a erickson
 * SW7425-4058: reduce WRN chatter for frame advance
 * 
 * 48   3/28/12 6:01p bandrews
 * SW7425-2731: override sync mute if entering trick mode
 * 
 * 47   3/14/12 12:34p erickson
 * SW7401-4517: remove old code
 * 
 * 46   3/6/12 8:53a erickson
 * SW7425-2404: add NEXUS_VideoDecoderTrickState.maxFrameRepeat
 * 
 * 45   3/2/12 6:31p bandrews
 * SW7425-2276: make underflow mode message warning
 * 
 * 44   3/2/12 5:00p bandrews
 * SW7425-2276: merge to main
 * 
 * 43   3/2/12 4:54p jtna
 * SW7425-2507: remove DSS check. XVD always expects it in 45KHz units
 * 
 * 42   3/1/12 4:12p erickson
 * SW7425-2527: disable userdata in host trick mode. it will be corrupted.
 * 
 * 41   2/29/12 4:11p jtna
 * SW7425-2507: take XVD's default for SetVeryLateThreshold() in non-trick
 *  mode
 * 
 * SW7425-2276/1   3/1/12 7:06p bandrews
 * SW7425-2276: improve gap detection; use correct placement for BXVD_PVR_
 *  function call; ensure all counters are 64 bit
 * 
 * 40   1/25/12 11:20a erickson
 * SW7425-2214: expand early and late thresholds when in SW STC fast trick
 *  mode
 * 
 * 39   10/27/11 9:21a erickson
 * SW7425-1264: update NEXUS_TsmMode_eSimulated
 * 
 * 38   10/24/11 2:05p erickson
 * SW7425-1264: update NEXUS_TsmMode_eSimulated
 * 
 * 37   10/12/11 10:21a erickson
 * SW7425-1264: add NEXUS_TsmMode_eSimulated
 * 
 * 36   4/14/11 3:31p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 * 
 * 35   1/4/11 10:10a erickson
 * SW7400-2842: remove NEXUS_VIDEO_DECODER_SINGLEDECODE
 *
 * 34   10/4/10 10:06a erickson
 * SWDEPRECATED-1003: update api
 *
 * 33   9/28/10 1:57p erickson
 * SWDEPRECATED-1003: add
 *  NEXUS_VideoDecoderTrickState.forceSourceFrameRate
 *
 * 32   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 31   9/1/10 10:31a erickson
 * SW7400-2870: restore old code for #if NEXUS_VIDEO_DECODER_SINGLEDECODE
 *
 * 30   8/6/10 12:31p erickson
 * SW7400-2870: use BXVD_SetPlaybackRate for finer grained control
 *
 * 29   7/15/10 6:31p vsilyaev
 * SW7400-2853: Use BXVD_SetVsyncPlaybackRate to set
 *  slowmotion/fastforward rate
 *
 * 28   7/7/10 11:47a mward
 * SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
 *  singledecode on 97400.
 *
 * 27   6/29/10 2:57p erickson
 * SW7405-4367: remove NEXUS_VideoDecoder_Flush. a proper flush must be
 *  done by the higher level SW so that it can re-seek to current
 *  position. also, a flush to avoid macroblockingn for I/IP->All appears
 *  unnecessary.
 *
 * 26   9/30/09 6:48p vsilyaev
 * SW7405-2732: Added fragmented mode, for the decoder side it set using
 *  Reoorder and filtering I frames only
 *
 * 25   9/30/09 3:40p erickson
 * SW7405-2880: add comment to better define the meaning of
 *  BXVD_SetDiscardThreshold
 *
 * 24   8/14/09 4:45p erickson
 * PR57700: add NEXUS_VideoDecoderSettings.discardThreshold
 *
 * 23   4/15/09 4:22p jtna
 * PR53781: check for DSS and don't assume 45KHz
 *
 * 22   4/1/09 3:12p erickson
 * PR53653: fix warning
 *
 * 21   4/1/09 12:54p erickson
 * PR53653: get default discard threshold from XVD instead of duplicating
 *  values in nexus
 *
 * 20   3/31/09 7:29p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 *
 * 19   3/17/09 2:46p erickson
 * PR53273: switch to BXVD_SkipMode_eDecode_Ref_Only for
 *  NEXUS_VideoDecoderDecodeMode_eIP at the AVD FW team's request
 *
 * 18   3/6/09 2:00p erickson
 * PR52891: trick mode functions should fail if VideoDecoder is not
 *  started. testing if it is connected is sufficient.
 *
 * 17   2/5/09 9:45a erickson
 * PR51720: back out manual two-field frame advance.
 *  BXVD_PVR_FrameAdvanceMode_eFrame_by_Frame will now do it.
 *
 * 16   1/29/09 4:43p erickson
 * PR51720: frame-by-frame advance of interlaced source when not top-
 *  field-only doesn't look good. instead, advance a top and bottom pair.
 *
 * 15   1/22/09 10:40a erickson
 * PR51134: added NEXUS_VideoDecoderTrickState.fieldByFieldAdvance
 *
 * 14   12/18/08 3:26p erickson
 * PR50541: back out change to BXVD_PVR_SetHostSparseMode logic. original
 *  problem fixed in nexus_playback_trick.c.
 *
 * 13   11/21/08 1:52p katrep
 * PR49042: Must enable HostSparseMode during AVC host and brcm
 *  trickmodes.
 *
 * 12   11/19/08 1:28p erickson
 * PR48744: back out needed code
 *
 * 11   11/18/08 3:09p maivu
 * PR 47967: undo 3548/3556 B0 AVD/MEMC hang
 *
 * 10   11/12/08 5:42p erickson
 * PR 48844: must flush the decoder when skip mode changes, also fixed B0
 *  workaround logic
 *
 * 9   11/9/08 11:17a maivu
 * PR47967: Added run-time option to force i-only for "iframe_only" and
 *  "iframe_mpeg2"
 *
 * 8   11/5/08 11:11a erickson
 * PR47967: added AVD/MEMC 3548/3556 B0 workaround for DivX
 *
 * 7   10/29/08 10:05a erickson
 * PR48406: allow NEXUS_VideoDecoder_SetTrickState to be called during
 *  normal play with no update, no glitch.
 *  NEXUS_VideoDecoder_P_Trick_Reset should reset to normal play (no
 *  trick) status.
 *
 * 6   10/23/08 4:21p jtna
 * PR47758: NEXUS_VideoDecoder_GetNextPts() now returns nextPTS, not PTS
 *  increment
 *
 * 5   10/1/08 4:41p jtna
 * PR37222: Add NEXUS_VideoDecoder_GetNextPts
 *
 * 4   5/20/08 1:56p erickson
 * PR42858: the default trick state is TSM enabled (that is, no trick
 *  mode)
 *
 * 3   5/16/08 11:04a erickson
 * PR42751: centralize logic for calling BXVD_SetVideoDisplayMode so that
 *  all aspects can be considered in one spot
 *
 * 2   5/5/08 10:32a vsilyaev
 * PR 42355: Added debug features
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/7   11/13/07 11:56a erickson
 * PR36068: prevent divide by zero
 *
 * Nexus_Devel/6   11/1/07 9:06a erickson
 * PR36633: update base enums
 *
 * Nexus_Devel/5   10/17/07 2:07p erickson
 * PR34926: rework NEXUS_VideoDecoderHandle to internally point to an XVD
 * channel, not device
 *
 * Nexus_Devel/4   10/17/07 1:05p vsilyaev
 * PR 35824: Added playpump_trick
 *
 * Nexus_Devel/3   10/16/07 4:54p vsilyaev
 * PR 35824: Added implementation of trickmode extension
 *
 * Nexus_Devel/2   10/16/07 3:43p vsilyaev
 * PR 35824: Added trickmode extension
 *
 **************************************************************************/

#include "nexus_video_decoder_module.h"
#include "bxvd_pvr.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_video_decoder_trick);

void
NEXUS_VideoDecoder_P_GetTrickState_Common(NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderTrickState *pState)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_ASSERT(pState);
    *pState = videoDecoder->trickState;
    return ;
}

static NEXUS_Error
NEXUS_VideoDecoder_P_SetDiscardThreshold(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_Error rc;
    unsigned discardThreshold;
    unsigned veryLateThreshold = videoDecoder->defaultVeryLateThreshold; /* take XVD's default */
    unsigned units = 45000; /* PTS units for 1 second. XVD always expects it in 45KHz units, even for DSS */

    if (videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eSimulated) {
        /* expand the normal play threshold by a factor of 'rate' */
        int rate = videoDecoder->settings.discardThreshold ? videoDecoder->settings.discardThreshold : videoDecoder->defaultDiscardThreshold;
        unsigned numerator = videoDecoder->trickState.rate > 0 ? videoDecoder->trickState.rate : -videoDecoder->trickState.rate;
        if (numerator > NEXUS_NORMAL_DECODE_RATE) { /* never set less than normal play */
            rate = rate / NEXUS_NORMAL_DECODE_RATE * numerator; 
        }
        discardThreshold = rate;
        veryLateThreshold = rate;
    }
    /* Allow the user to override */
    else if (videoDecoder->settings.discardThreshold) {
        discardThreshold = videoDecoder->settings.discardThreshold;
    }
    /* For host paced trick modes, we need a discard threshold >= the time between I frames. */
    else if (videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eEnabled && (videoDecoder->trickState.hostTrickModesEnabled || videoDecoder->trickState.brcmTrickModesEnabled)) {
        discardThreshold = 30 * units;
    }
    else {
        discardThreshold = videoDecoder->defaultDiscardThreshold;
    }

    BDBG_MSG(("set thresholds: discard %d, verylate %d", discardThreshold, veryLateThreshold));
    /* BXVD_SetDiscardThreshold sets the "early" discard threshold. That is, if a picture arrives in the DM which is earlier than
    this threshold, the DM will drop it. If you set this threshold too large, then the DM could get stuck on PTS discontinuities. */
    rc = BXVD_SetDiscardThreshold(videoDecoder->dec, discardThreshold);
    if (rc) return BERR_TRACE(rc);
    
    /* the "very late" threshold is simply the adjustable late threshold. this is only modified for SW STC trick. */
    BXVD_SetVeryLateThreshold(videoDecoder->dec, veryLateThreshold);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

NEXUS_Error
NEXUS_VideoDecoder_P_SetTrickState_Avd(NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderTrickState *pState)
{
    NEXUS_Error rc;
    BXVD_ChannelHandle dec;
    BXVD_PlaybackRateSettings rate;
    BXVD_FrameRateSettings frameRateOverrideSettings;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_ASSERT(pState);

    if (!videoDecoder->dec) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    dec = videoDecoder->dec;
#if NEXUS_OTFPVR
    if(videoDecoder->startSettings.codec == NEXUS_VideoCodec_eMpeg2) {
        bool complete;
        rc = NEXUS_VideoDecoder_P_OtfPvr_SetTrickState(videoDecoder, pState, &complete);
        if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
        if(complete) { return NEXUS_SUCCESS; }
        /* keep going */
    }
#endif /*  NEXUS_OTFPVR */
    if(
        (pState->reorderingMode==NEXUS_VideoDecoderReorderingMode_eChunkForward ||
         pState->reorderingMode==NEXUS_VideoDecoderReorderingMode_eChunkBackward) &&
         pState->decodeMode == NEXUS_VideoDecoderDecodeMode_eI) {
        videoDecoder->errorHandlingOverride = true;
        videoDecoder->errorHandlingMode = NEXUS_VideoDecoderErrorHandling_ePicture;
    } else if(pState->reorderingMode!=NEXUS_VideoDecoderReorderingMode_eNone) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    } else {
        videoDecoder->errorHandlingOverride = false;
    }

    (void)BXVD_GetPlaybackRate(dec, &rate);
    if (pState->rate && pState->tsmEnabled != NEXUS_TsmMode_eSimulated) {
        rate.uiNumerator = pState->rate;
    }
    else {
        rate.uiNumerator = NEXUS_NORMAL_DECODE_RATE;
    }
    rate.uiDenominator = NEXUS_NORMAL_DECODE_RATE;
    rc = BXVD_SetPlaybackRate(dec, rate /* yes, pass by value */);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    (void)BXVD_GetFrameRateOverride(dec, &frameRateOverrideSettings);
    frameRateOverrideSettings.bValid = pState->forceSourceFrameRate != 0;
    frameRateOverrideSettings.bTreatAsSingleElement = frameRateOverrideSettings.bValid;
    if (frameRateOverrideSettings.bValid) {
        frameRateOverrideSettings.stRate.uiNumerator = pState->forceSourceFrameRate;
        frameRateOverrideSettings.stRate.uiDenominator = 1000;
    }
    rc = BXVD_SetFrameRateOverride(dec, &frameRateOverrideSettings);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    /* BDBG_MSG(("SparseMode %d",((pState->brcmTrickModesEnabled || pState->hostTrickModesEnabled) && (videoDecoder->startSettings.codec == NEXUS_VideoCodec_eH264)))); */
    rc = BXVD_PVR_SetHostSparseMode(dec, (pState->brcmTrickModesEnabled || pState->hostTrickModesEnabled) && videoDecoder->startSettings.codec == NEXUS_VideoCodec_eH264);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}
    rc = BXVD_PVR_EnableReverseFields(dec, pState->reverseFields);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}
    rc = BXVD_PVR_SetGopTrickMode(dec, pState->dqtEnabled);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    /* BTP's are always processed by RAVE and XVD. No setting for this. */

    rc = BXVD_SetDisplayFieldMode(dec, pState->topFieldOnly?BXVD_DisplayFieldType_eTopFieldOnly : BXVD_DisplayFieldType_eBothField);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    if (pState->decodeMode != videoDecoder->trickState.decodeMode) {
        /* A flush is recommended on skip mode transitions, but this must be done by higher level SW so that it can avoid a trick mode transition
        delay due to a full display queue. */
        rc = BXVD_SetSkipPictureModeDecode(dec,
            pState->decodeMode==NEXUS_VideoDecoderDecodeMode_eI?BXVD_SkipMode_eDecode_I_Only:
            (pState->decodeMode==NEXUS_VideoDecoderDecodeMode_eIP?BXVD_SkipMode_eDecode_Ref_Only: /* Ref_Only is preferred for IP trick modes, not _eIP_Only */
            BXVD_SkipMode_eDecode_IPB));
        if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}
    }

    rc = BXVD_PVR_EnablePause(dec, pState->rate==0);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    if (!pState->maxFrameRepeat) {
        videoDecoder->maxFrameRepeat.pictureDisplayCount = 0;
    }
    
    BKNI_EnterCriticalSection();
    videoDecoder->trickState = *pState;
    BKNI_LeaveCriticalSection();

    /* the following functions require coordination with other state variables in VideoDecoder */
    rc = NEXUS_VideoDecoder_P_SetDiscardThreshold(videoDecoder);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    rc = NEXUS_VideoDecoder_P_SetTsm(videoDecoder);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}
 
    rc = NEXUS_VideoDecoder_P_SetMute(videoDecoder);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    rc = BXVD_SetErrorHandlingMode(dec, videoDecoder->errorHandlingOverride?videoDecoder->errorHandlingMode:videoDecoder->startSettings.errorHandling);
    if (rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto err_settings;}

    /* disable userdata on host trick modes */
    NEXUS_VideoDecoder_P_SetUserdata(videoDecoder);
    
    return NEXUS_SUCCESS;
err_settings:
    return rc;
}

NEXUS_Error
NEXUS_VideoDecoder_P_FrameAdvance_Avd(NEXUS_VideoDecoderHandle videoDecoder)
{
    NEXUS_Error  rc;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (!videoDecoder->dec) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    /* for interlaced content not in topFieldOnly mode, BXVD_PVR_FrameAdvanceMode_eFrame_by_Frame will advance two fields. */
    rc = BXVD_PVR_FrameAdvance(videoDecoder->dec,
        videoDecoder->trickState.fieldByFieldAdvance ? BXVD_PVR_FrameAdvanceMode_eField_by_Field : BXVD_PVR_FrameAdvanceMode_eFrame_by_Frame);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_VideoDecoder_P_GetNextPts_Avd(NEXUS_VideoDecoderHandle videoDecoder, uint32_t *pNextPts)
{
    BERR_Code rc;
    BXVD_PTSInfo nextPtsInfo;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (!videoDecoder->dec) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    rc = BXVD_GetNextPTS_isr(videoDecoder->dec, &nextPtsInfo);

    if ((nextPtsInfo.ePTSType==(BXDM_PictureProvider_PTSType)BXVD_PTSType_eInterpolatedFromInvalidPTS) || (rc!=BERR_SUCCESS)) {
        BDBG_MSG(("NEXUS_VideoDecoder_GetNextPts: Next PTS is invalid"));
        *pNextPts = 0; /* If the next PTS is invalid, we return 0 */
        return NEXUS_UNKNOWN;
    }
    else {
        *pNextPts = nextPtsInfo.ui32EffectivePTS;
        BDBG_MSG(("NEXUS_VideoDecoder_GetNextPts: Next PTS=%08X", *pNextPts));
        return NEXUS_SUCCESS;
    }
}

void
NEXUS_VideoDecoder_GetNormalPlay(NEXUS_VideoDecoderTrickState *pState)
{
    BKNI_Memset(pState, 0, sizeof(*pState));
    pState->rate = NEXUS_NORMAL_DECODE_RATE;
    pState->decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
    pState->tsmEnabled = NEXUS_TsmMode_eEnabled;
    pState->reorderingMode = NEXUS_VideoDecoderReorderingMode_eNone;
}

void
NEXUS_VideoDecoder_P_Trick_Reset_Generic(NEXUS_VideoDecoderHandle videoDecoder)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_VideoDecoder_GetNormalPlay(&videoDecoder->trickState);
    return;
}

NEXUS_Error NEXUS_VideoDecoder_InvalidateSimulatedStc( NEXUS_VideoDecoderHandle videoDecoder )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (videoDecoder->trickState.tsmEnabled != NEXUS_TsmMode_eSimulated) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    return BXVD_SetSTCInvalidFlag(videoDecoder->dec, true); /* true means it is invalid */
}

NEXUS_Error NEXUS_VideoDecoder_P_SetSimulatedStc_isr(NEXUS_VideoDecoderHandle videoDecoder, uint32_t pts)
{
    BXVD_ClockOverride clockOverride;
    NEXUS_Error rc;
        
    BDBG_ASSERT(videoDecoder->trickState.tsmEnabled == NEXUS_TsmMode_eSimulated);
    
    (void)BXVD_GetClockOverride_isr(videoDecoder->dec, &clockOverride);
    clockOverride.uiStcValue = pts;
    clockOverride.bLoadSwStc = true;
    rc = BXVD_SetClockOverride_isr(videoDecoder->dec, &clockOverride);
    if (rc) return BERR_TRACE(rc);
    BDBG_MSG(("set simulated stc %08x (%d %d)", pts, clockOverride.bEnableClockOverride, clockOverride.iStcDelta));
    
    (void)BXVD_SetSTCInvalidFlag_isr(videoDecoder->dec, false); /* false means it is valid */
    
    return 0;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetUnderflowMode_isr(NEXUS_VideoDecoderHandle videoDecoder, bool underflowMode)
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    BDBG_WRN(("Set video underflow mode %s", underflowMode ? "on" : "off"));
    rc = BXVD_PVR_SetIgnoreNRTUnderflow_isr(videoDecoder->dec, underflowMode);

    return rc;
}

