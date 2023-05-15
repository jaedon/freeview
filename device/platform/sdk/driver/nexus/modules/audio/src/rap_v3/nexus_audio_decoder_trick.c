/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_audio_decoder_trick.c $
 * $brcm_Revision: 21 $
 * $brcm_Date: 9/30/11 5:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/7405/src/nexus_audio_decoder_trick.c $
 * 
 * 21   9/30/11 5:31p jgarrett
 * SW7425-1297: Changes for 0.5x..2.0x audio trick modes and automatic
 *  muting on unsupported rates
 * 
 * SW7425-1297/1   9/28/11 6:29p jgarrett
 * SW7425-1297: Adding automatic mute on unsupported trick rates
 * 
 * 20   4/27/11 5:03p jgarrett
 * SW7405-5192: Enabled changing audio trick rate on the fly
 * 
 * 19   4/27/11 4:19p jgarrett
 * SW7420-1826: Adding AudioOutputGroup Interface
 * 
 * 18   8/3/10 1:37p akam
 * SW35230-211: Update for 35230 RAP PI changes.
 * 
 * 17   5/14/10 3:40p akam
 * SW35230-48: Fix Nexus audio build for 35230.
 * 
 * 16   5/13/10 6:32p jgarrett
 * SW7405-4168: Adding variable rate DSOLA
 * 
 * SW7405-4168/1   4/14/10 11:58a jgarrett
 * SW7405-4168: Adding variable DSOLA rate support
 * 
 * 15   1/9/10 9:42a jgarrett
 * SW3548-2710: Clearing STC trick flag on reset
 * 
 * 14   2/12/09 5:36p jgarrett
 * PR 46135: Adding TruVolume
 * 
 * 13   1/15/09 3:18p jgarrett
 * PR 51153: Allowing for stop without flush in trick transitions
 * 
 * 12   1/15/09 11:08a jgarrett
 * PR 50573: Fixing advance through discontinuities
 * 
 * 11   11/4/08 5:54p jgarrett
 * PR 48184: Fixing error code if advance is called prior to PTS being
 *  received
 * 
 * 10   10/29/08 6:22p jgarrett
 * PR 48518: Validating TSM flags between ASTM and trick/stcChannel
 * 
 * 9   10/28/08 3:26p jgarrett
 * PR 47673: Fixing audio mute state with some trick transitions
 * 
 * 8   9/24/08 6:56p katrep
 * PR46562: Fixed loss audio after the trickmodes
 * 
 * 7   9/24/08 5:52p katrep
 * PR46562: fixed pause from 120x PB
 * 
 * 6   9/24/08 1:59p jgarrett
 * PR 46562: Revising pause logic with FSPB.
 * 
 * 5   9/18/08 6:28p katrep
 * PR45880: Add support for FSPB(80/120 playback)
 * 
 * 4   5/22/08 8:28p jgarrett
 * PR 42974: Adding mute flag
 * 
 * 3   4/7/08 12:51p jgarrett
 * PR 40120: Removing warning message for 3563
 * 
 * 2   3/14/08 10:38a jgarrett
 * PR 40120: Adding TSM option
 * 
 * 1   1/18/08 2:22p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/5   12/5/07 4:30p erickson
 * PR36404: fix BRAP_EnablePvrPause call
 * 
 * Nexus_Devel/4   12/4/07 2:30p vsilyaev
 * PR 36404: Use structure to conrol audio trickmodes
 * 
 * Nexus_Devel/3   11/29/07 2:25p erickson
 * PR35457: doc update for 0.5 release
 * 
 * Nexus_Devel/2   11/12/07 6:49p jgarrett
 * PR 36953: Stubbing trick functions on 3563
 * 
 * Nexus_Devel/1   11/9/07 9:59a jgarrett
 * PR 36774: Adding 7405 support
 * 
 * Nexus_Devel/3   11/6/07 4:23p jgarrett
 * PR 34954: Migrating to audio connector model
 * 
 * Nexus_Devel/2   10/24/07 5:40p vsilyaev
 * PR 36404: Added audio decode trick mode
 * 
 * Nexus_Devel/1   10/24/07 12:36p vsilyaev
 * PR 36404: Trick mode extension
 * 
 * Nexus_Devel/4   10/11/07 9:23a erickson
 * PR36017: added BRAP_GetDefaultSettings, clean up on Uninit
 * 
 * Nexus_Devel/3   9/28/07 5:14p jgarrett
 * PR 34594: Adding sync thunks
 * 
 * Nexus_Devel/2   9/27/07 9:19p jgarrett
 * PR 34954: Adding SPDIF
 * 
 * Nexus_Devel/1   9/27/07 7:52p jgarrett
 * PR 34594: Adding module calls
 * 
 **************************************************************************/
#include "nexus_audio_module.h"
#include "nexus_audio_priv.h"
#include "bpcrlib.h"

BDBG_MODULE(nexus_audio_decoder_trick);

void NEXUS_AudioDecoder_GetTrickState(
    NEXUS_AudioDecoderHandle decoder,
    NEXUS_AudioDecoderTrickState *pState 
    )
{
    BDBG_ASSERT(decoder);
    BDBG_ASSERT(pState);
    *pState = decoder->trickState;
    return;
}

NEXUS_Error NEXUS_AudioDecoder_SetTrickState(
    NEXUS_AudioDecoderHandle decoder,             
    const NEXUS_AudioDecoderTrickState *pTrickState
    )
{
    NEXUS_Error rc;
    bool forceMute;

    BDBG_ASSERT(decoder);
    BDBG_ASSERT(pTrickState);

    if( !decoder->started ) 
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("muted %d,force stopped %d,rate %d",pTrickState->muted,pTrickState->forceStopped,pTrickState->rate));

    if ( (pTrickState->rate < 500 || pTrickState->rate > 2000) && pTrickState->rate != 0 )
    {
        BDBG_MSG(("Unsupported trick rate.  Muting decoder output."));
        forceMute = true;
    }
    else
    {
        forceMute = false;
    }

    rc = NEXUS_AudioDecoder_P_SetTrickMute(decoder, pTrickState->muted || forceMute);
    if(rc!=NEXUS_SUCCESS) 
    {
        rc=BERR_TRACE(rc);
        goto err_set_state;
    }

    if ( decoder->trickForceStopped != pTrickState->forceStopped ||
         decoder->trickState.rate != pTrickState->rate )
    {    
        BRAP_DSPCHN_PlayBackRate oldRate = decoder->audioParams.sDspChParams.uiPBRate;

        if ( pTrickState->rate >= (NEXUS_NORMAL_DECODE_RATE/2) && pTrickState->rate <= (NEXUS_NORMAL_DECODE_RATE*2) )
        {
            decoder->audioParams.sDspChParams.uiPBRate = (pTrickState->rate * BRAP_DSPCHN_PLAYBACKRATE_NORMAL)/NEXUS_NORMAL_DECODE_RATE;
        }

        if ( pTrickState->forceStopped ) 
        {
            BDBG_MSG(("Stopping audio decode"));
            rc = NEXUS_AudioDecoder_P_Stop(decoder, true);
        } 
#if BRAP_VER == 3  /* Function not yet defined */
        else if ( oldRate != decoder->audioParams.sDspChParams.uiPBRate && decoder->audioParams.sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_eDecode )
        {
            if ( oldRate == BRAP_DSPCHN_PLAYBACKRATE_NORMAL || 
                 decoder->audioParams.sDspChParams.uiPBRate == BRAP_DSPCHN_PLAYBACKRATE_NORMAL )
            {
                /* We are entering or leaving a trick mode.  Must stop/start */
                BDBG_MSG(("Restarting audio decode"));
                (void)NEXUS_AudioDecoder_P_Stop(decoder, false);
                rc = NEXUS_AudioDecoder_P_Start(decoder);
            }
            else
            {
                /* Trick -> Trick */
                BDBG_MSG(("Updating trick rate from %u -> %u", oldRate, decoder->audioParams.sDspChParams.uiPBRate));
                rc = BRAP_SetAudioPlayRate(decoder->rapChannel, decoder->audioParams.sDspChParams.uiPBRate);
            }
        }
#else
        else if ( oldRate != decoder->audioParams.sDspChParams.uiPBRate && decoder->audioParams.sDspChParams.eDecodeMode == BRAP_DSPCHN_DecodeMode_eDecode )
        {
            BDBG_MSG(("Restarting audio decode"));
            (void)NEXUS_AudioDecoder_P_Stop(decoder, false);
            rc = NEXUS_AudioDecoder_P_Start(decoder);
        }
#endif
        else if (!pTrickState->forceStopped && decoder->trickForceStopped )
        {
            BDBG_MSG(("Starting audio decode"));
            rc = NEXUS_AudioDecoder_P_Start(decoder);
        }

        if(rc!=NEXUS_SUCCESS) 
        {
            rc=BERR_TRACE(rc);
            goto err_set_state;
        }
        decoder->trickForceStopped = pTrickState->forceStopped;
    }

    BDBG_ASSERT(decoder->rapChannel);
    if(!decoder->trickForceStopped) {
#if BCHP_CHIP == 3563
        if(0 == pTrickState->rate) {
            /* No pause functionality supported at the moment... */
            rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        }
#else
        rc = BRAP_EnablePvrPause(decoder->rapChannel, pTrickState->rate==0);
        if(rc!=NEXUS_SUCCESS) 
        {
            rc=BERR_TRACE(rc);
            goto err_set_state;
        }
#endif
    }
    decoder->trickState = *pTrickState;
    NEXUS_AudioDecoder_P_SetTsm(decoder);
    return BERR_SUCCESS;
err_set_state:
    return rc;
}

/***************************************************************************
Summary:
    Controls pause state of the audio decoder
See Also:
    NEXUS_AudioDecoder_Start
    NEXUS_AudioDecoder_Advance
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_Pause(
    NEXUS_AudioDecoderHandle decoder,             
    bool enable
    )
{
#if BCHP_CHIP == 3563
/* WARNING: 3563 RAP has not implemented BRAP_EnablePvrPause */
    BSTD_UNUSED(decoder);
    BSTD_UNUSED(enable);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#else
    BERR_Code rc;

    BDBG_ASSERT(decoder);

    if( !decoder->started ) 
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( !decoder->running )
    {
        return BERR_SUCCESS;
    }

    rc = BRAP_EnablePvrPause(decoder->rapChannel, enable);
    if ( rc ) 
    {
        return BERR_TRACE(rc);
    }

    return NEXUS_SUCCESS;
#endif
}

/***************************************************************************
Summary:
    Advances audio decoder to the target PTS
Desctiption:
    NEXUS_AudioDecoder_Advance causes audio decoder to drop compressed data,
    untill it finds data with timestamp greater then target PTS.

See Also:
    NEXUS_AudioDecoder_Pause
 ***************************************************************************/
NEXUS_Error NEXUS_AudioDecoder_Advance(
    NEXUS_AudioDecoderHandle decoder,             
    uint32_t targetPts /* PTS in 45Khz (MPEG-2) or 27MHz domain */
    )
{
#if BCHP_CHIP == 3563
/* WARNING: 3563 RAP has not implemented BRAP_EnablePvrPause */
    BSTD_UNUSED(decoder);
    BSTD_UNUSED(targetPts);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
#else
    BERR_Code rc;
    uint32_t audio_pts;
    BRAP_ChannelHandle channel;
    BRAP_DSPCHN_PtsInfo ptsInfo;

    BDBG_ASSERT(decoder);

    if( !decoder->started ) 
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( !decoder->running )
    {
        return BERR_SUCCESS;
    }

    BDBG_ASSERT(decoder->rapChannel);
    channel = decoder->rapChannel;

    rc = BRAP_GetCurrentPTS(channel, &ptsInfo);
    if ( rc ) 
    {
        if ( rc == BRAP_ERR_BAD_DEVICE_STATE )
        {
            /* PTS not available yet.  Don't treat this as an error case. */
            return BERR_SUCCESS;
        }
        else
        {
            return BERR_TRACE(rc);
        }
    }
    if ( ptsInfo.ePtsType != BRAP_DSPCHN_PtsType_eInterpolatedFromInvalidPTS ) 
    {
        audio_pts = ptsInfo.ui32RunningPts;
    } 
    else 
    {
        audio_pts = 0;
    }

    if ( targetPts == 0 || audio_pts == 0 ) 
    {
        /* If there's no video or audio, we still need to advance a bit.
        If video is 0, we can't allow audio to hold playback and lock up the state machine.
        If audio is 0, we need the decoder to consume a little bit so it can get a PTS. */
        rc = BRAP_PvrFrameAdvance(channel, 10);
        if ( rc ) 
        {
            return BERR_TRACE(rc);
        }
    } 
    else
    {
        int32_t diff; /* in 22.5 KHz units */
        diff = BPCRlib_StcDiff_isr(decoder->isDss, targetPts, audio_pts);
        if (diff > 0) 
        {   
            diff /= 23; /* convert from 22.5Khz to milliseconds */
        
            /* don't advance unless we have a complete frame (50 milliseconds handles NTSC and PAL). this
                minimizes calls to RAP FW. */
            if (diff >= 50) 
            {
                BDBG_MSG(("frameadv[%#lx]: vpts=%#x, apts=%#x (diff %d), adv %d msec", (unsigned long)decoder, targetPts, audio_pts, targetPts-audio_pts, diff));
                rc = BRAP_PvrFrameAdvance(channel, diff);
                if ( rc ) 
                {
                    return BERR_TRACE(rc);
                }
            }
        } 
        else if (diff < -(16*22500))
        {
            /* if we're way behind, just consume a bit and expect we will get through it */
            rc = BRAP_PvrFrameAdvance(channel, 1500);
            if ( rc )
            {
                return BERR_TRACE(rc);
            }
        }
        else 
        {
            BDBG_MSG(("frameadv[%#lx]: vpts=%#x,apts=%#x (diff %d), no advance", (unsigned long)decoder, targetPts,audio_pts, diff));
        }
    }
    return BERR_SUCCESS;
#endif
}

void NEXUS_AudioDecoder_P_TrickReset( NEXUS_AudioDecoderHandle decoder)
{
    BDBG_ASSERT(decoder);
    NEXUS_AudioDecoder_P_SetTrickMute(decoder, false);
    decoder->trickState.forceStopped = false;
    decoder->trickState.rate = NEXUS_NORMAL_DECODE_RATE;
    decoder->trickState.tsmEnabled = true;
    decoder->trickState.muted = false;
    decoder->trickState.stcTrickEnabled = false;
}

