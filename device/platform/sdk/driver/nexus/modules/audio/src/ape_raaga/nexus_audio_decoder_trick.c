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
 * $brcm_Workfile: nexus_audio_decoder_trick.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 4/23/12 6:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/7422/src/nexus_audio_decoder_trick.c $
 * 
 * 7   4/23/12 6:42p bandrews
 * SW7346-795: reapply rate control settings if audio was restarted from
 *  change in forceStopped flag
 * 
 * 6   2/17/12 5:12p jgarrett
 * SW7358-198: Adding error return
 * 
 * 5   9/30/11 5:31p jgarrett
 * SW7425-1297: Changes for 0.5x..2.0x audio trick modes and automatic
 *  muting on unsupported rates
 * 
 * SW7425-1297/1   9/29/11 9:34a jgarrett
 * SW7425-1297: Fixing link error with SetTrickMute
 * 
 * 4   8/9/11 4:57p jgarrett
 * SW7346-412: ResetTrickState clears stcTrick flag instead of setting it
 * 
 * 3   4/29/11 5:21p jgarrett
 * SW7425-471: Fixing entry to pause state for audio trick modes
 * 
 * 2   3/23/11 4:29p jgarrett
 * SW7422-403: Fixing sync mute and DSOLA
 * 
 * 1   12/17/10 3:56p jgarrett
 * SW7422-146: Adding initial nexus on APE for 7422
 * 
 **************************************************************************/
#include "nexus_audio_module.h"
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
    bool rateChanged;
    NEXUS_AudioDecoderTrickState oldState;

    BDBG_ASSERT(decoder);
    BDBG_ASSERT(pTrickState);

    if( !decoder->started ) 
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    BDBG_MSG(("muted %d,force stopped %d,rate %d",pTrickState->muted,pTrickState->forceStopped,pTrickState->rate));

    rateChanged = (decoder->trickState.rate != pTrickState->rate) ?true:false;
    if ( decoder->trickForceStopped != pTrickState->forceStopped )
    {    
        if ( pTrickState->forceStopped ) 
        {
            BDBG_MSG(("Stopping audio decode"));
            rc = NEXUS_AudioDecoder_P_Stop(decoder, true);
            if(rc!=NEXUS_SUCCESS) 
            {
                rc=BERR_TRACE(rc);
                goto err_set_state;
            }
        } 
        else if (!pTrickState->forceStopped && decoder->trickForceStopped )
        {
            BDBG_MSG(("Starting audio decode"));
            rc = NEXUS_AudioDecoder_P_Start(decoder);
            if(rc!=NEXUS_SUCCESS) 
            {
                rc=BERR_TRACE(rc);
                goto err_set_state;
            }
        }

        decoder->trickForceStopped = pTrickState->forceStopped;
        rateChanged = true;
    }
    if ( !decoder->trickForceStopped && rateChanged )
    {
        bool wasTrick, isTrick;
        bool wasPause, isPause;
        wasTrick = decoder->apeStartSettings.decodeRateControl;
        wasPause = (decoder->trickState.rate == 0) ? true : false;
        isPause = (pTrickState->rate == 0) ? true : false;
        switch ( pTrickState->rate )
        {
        case 0:
        case NEXUS_NORMAL_DECODE_RATE:
            isTrick = false;
            break;
        default:
            isTrick = true;
            break;
        }
        /* Valid Rate Changes: */
        /* 1 Normal --> Pause */
        /* 2 Normal --> Trick */
        /* 3 Pause --> Normal */
        /* 4 Pause --> Trick */
        /* 5 Trick --> Normal */
        /* 6 Trick --> Pause */

        /* Handle rate changes */
        if ( isPause )
        {
            /* 1 Normal --> Pause */
            /* 6 Trick --> Pause */
            rc = BAPE_Decoder_Pause(decoder->channel);
            if(rc!=NEXUS_SUCCESS) 
            {
                rc=BERR_TRACE(rc);
                goto err_set_state;
            }
        }
        else
        {
            if ( isTrick != wasTrick )
            {
                /* 2 Normal --> Trick */
                /* 4 Pause --> Trick (was paused from normal [1]) */
                /* 5 Trick --> Normal */
                NEXUS_AudioDecoder_P_Stop(decoder, false);
                decoder->apeStartSettings.decodeRateControl = isTrick;
                rc = NEXUS_AudioDecoder_P_Start(decoder);
                if(rc!=NEXUS_SUCCESS) 
                {
                    rc=BERR_TRACE(rc);
                    goto err_set_state;
                }
            }
            else if ( wasPause )
            {
                /* 3 Pause --> Normal */
                /* 4 Pause --> Trick (was paused from trick [6]) */
                rc = BAPE_Decoder_Resume(decoder->channel);                
                if(rc!=NEXUS_SUCCESS) 
                {
                    rc=BERR_TRACE(rc);
                    goto err_set_state;
                }
            }
        }               
    }

    oldState = decoder->trickState;
    decoder->trickState = *pTrickState;    
    rc = NEXUS_AudioDecoder_ApplySettings_priv(decoder); /* Apply trick mute and rate */
    if ( rc )
    {
        rc = BERR_TRACE(rc);
        goto err_apply_settings;
    }
    NEXUS_AudioDecoder_P_SetTsm(decoder);           /* Apply TSM setting */
    return BERR_SUCCESS;
err_apply_settings:
    decoder->trickState = oldState;
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

    if ( enable )
    {
        rc = BAPE_Decoder_Pause(decoder->channel);
    }
    else
    {
        rc = BAPE_Decoder_Resume(decoder->channel);
    }
    if ( rc ) 
    {
        return BERR_TRACE(rc);
    }

    return NEXUS_SUCCESS;
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
    BERR_Code rc;
    uint32_t audio_pts;
    BAPE_DecoderHandle channel;
    BAPE_DecoderTsmStatus tsmStatus;

    BDBG_ASSERT(decoder);

    if( !decoder->started ) 
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( !decoder->running )
    {
        return BERR_SUCCESS;
    }

    BDBG_ASSERT(decoder->channel);
    channel = decoder->channel;

    BAPE_Decoder_GetTsmStatus(channel, &tsmStatus);
    if ( tsmStatus.ptsInfo.ePTSType != BAVC_PTSType_eInterpolatedFromInvalidPTS )
    {
        audio_pts = tsmStatus.ptsInfo.ui32CurrentPTS;
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
        rc = BAPE_Decoder_Advance(channel, 10);
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
                rc = BAPE_Decoder_Advance(channel, diff);
                if ( rc ) 
                {
                    return BERR_TRACE(rc);
                }
            }
        } 
        else if (diff < -(16*22500))
        {
            /* if we're way behind, just consume a bit and expect we will get through it */
            rc = BAPE_Decoder_Advance(channel, 1500);
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
}

void NEXUS_AudioDecoder_P_TrickReset( NEXUS_AudioDecoderHandle decoder)
{
    BDBG_ASSERT(decoder);
    decoder->trickState.forceStopped = false;
    decoder->trickState.rate = NEXUS_NORMAL_DECODE_RATE;
    decoder->trickState.tsmEnabled = true;
    decoder->trickState.muted = false;
    decoder->trickState.stcTrickEnabled = false;
    if ( decoder->channel )
    {
        NEXUS_AudioDecoder_ApplySettings_priv(decoder);
    }
}

