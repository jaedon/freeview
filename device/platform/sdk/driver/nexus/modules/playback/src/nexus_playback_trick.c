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
 * $brcm_Workfile: nexus_playback_trick.c $
 * $brcm_Revision: 68 $
 * $brcm_Date: 10/12/12 12:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/playback/src/nexus_playback_trick.c $
 * 
 * 68   10/12/12 12:48p erickson
 * SW7435-393: extend MSG debug
 * 
 * 67   9/4/12 4:46p erickson
 * SW7425-3594: support simulated_tsm mode from bmedia_player_nav
 *
 * 66   6/14/12 2:35p erickson
 * SW7425-2741: add AudioDecoderPrimer
 *
 * SW7425-2741/1   6/7/12 7:18p ahulse
 * SW7425-2741: add audio primer
 *
 * 65   5/17/12 8:21p bandrews
 * SWDTV-4625: restrict stc invalidate call to non-stc-trick modes
 *
 * 64   1/25/12 1:09p erickson
 * SW7420-2215: set STC to normal rate using b_play_trick_stc_normal() in
 *  all cases except STC trick mode
 *
 * 63   1/2/12 11:27a erickson
 * SW7420-1965: return primary and secondary audio decoder status and
 *  trick state. make primary and simple decoder state mutually exclusive.
 *
 * 62   11/4/11 1:46p jgarrett
 * SW7344-221: Resetting audio mute when leaving pause for another trick
 *  mode
 *
 * 61   11/4/11 1:07p erickson
 * SW7231-442: audio trick state must be applied to all decoders
 *  associated with a pid channel
 *
 * 60   9/30/11 9:09a erickson
 * SW7405-5059: set decoder trick, then STC trick. allows STC trick to
 *  correctly verify decoder's TSM state.
 *
 * 59   9/29/11 3:29p ahulse
 * SW7425-1297: accurate calc of stc trick rate, mute doen by audio
 *  decoder
 *
 * 58   9/27/11 4:42p vsilyaev
 * SW7420-1965: Fixed code to fetch the audio decoder state
 *
 * 57   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 *
 * 56   9/7/11 11:41a erickson
 * SW7405-5059: fail stc trick if any decoder not in TSM mode
 *
 * 55   8/29/11 10:33a erickson
 * SW7425-1205: clarify trick DBG
 *
 * 54   5/24/11 4:53p vsilyaev
 * SWDTV-7161: Properly adjust the decoder_rate prior to applying STC rate
 *
 * 53   5/13/11 5:12p erickson
 * SW7125-823: add support for decoder-less host trick modes with
 *  nexus_playback
 *
 * 52   4/18/11 4:56p dlwin
 * SW7425-200: Fixed STC Trickmode, transition from Pause->HostPaced
 *  trickmode caused frozen video.
 *
 * 51   3/29/11 2:49p vsilyaev
 * SW7335-1214: Remove manual ModuleLock/Unlock
 *
 * 50   3/28/11 3:45p vsilyaev
 * SW7335-1214: Added NEXUS_CallbackHandler framework
 *
 * 49   2/10/11 5:14p erickson
 * SW7405-5059: only fail stc trick modes on ES streams when trick mode
 *  invoked
 *
 * 48   2/7/11 10:16a erickson
 * SW7405-5059: fail stc trick modes on ES streams
 *
 * 46   11/12/10 6:35p dlwin
 * SW35230-2135: Added code invalidate the STC channel on transition
 *  between non-STC trick modes.
 *
 * 45   9/28/10 1:57p erickson
 * SWDEPRECATED-1003: pass force source frame rate from media framework to
 *  VideoDecoder
 *
 * 44   8/17/10 12:24p erickson
 * SW3548-3051: add warning if trick mode performed with missing decoder
 *  handle
 *
 * 43   7/20/10 4:03p vsilyaev
 * SW7400-2853: Removed test that blocks decoder based fast forward
 *
 * 42   6/29/10 3:20p erickson
 * SW7405-4367: playback should flush when transitioning decoder skip
 *  modes. also added NEXUS_PlaybackTrickModeSettings.avoidFlush option to
 *  disable this behavior.
 *
 * 41   6/11/10 5:39p vsilyaev
 * SW3556-1123: Fixed warning
 *
 * 40   6/11/10 4:57p vsilyaev
 * SW3556-1123: use topFieldOnly mode when using host paced mode
 *
 * 39   3/26/10 1:00p vsilyaev
 * SW7550-349: Use offsetThreshold to guarantee STC update
 *
 * 38   12/3/09 11:18a mward
 * SW7125-58: SW7342-64: No longer mute audio with 0.8, 1.2x trick modes
 *  for 7125, 7342 RAP supports now DSOLA.
 *
 * 37   11/5/09 2:31p mward
 * SW7125-58: SW7342-64: Mute audio with 0.8, 1.2x trick modes for 7125,
 *  7342 until RAP supports DSOLA.
 *
 * 36   8/10/09 3:36p ahulse
 * PR56762: firstPts/Passed callbacks not registered in settop API mode,
 *  move registration into b_play_trick_set_pid
 *
 * 35   7/31/09 4:25p erickson
 * PR57235: rename Settop API names, remove unused code
 *
 * 34   7/21/09 5:57p vsilyaev
 * PR 56977: Nexus playback always control audio and video decoder.
 *  Provided way for the user to control the decoder slowmotion factor
 *  using the playback trick API.
 *
 * 33   7/8/09 11:59a vsilyaev
 * PR 55989: Added support for OTF trickmodes
 *
 * 32   6/19/09 4:11p vsilyaev
 * PR 56169: PR 53824: Allow user to sepcify max decodder rate. This
 *  superceeds overrideStcTrick.
 *
 * 31   3/6/09 1:58p erickson
 * PR52891: don't call decoders SetTrickState functions when they are not
 *  started
 *
 * 30   3/5/09 5:58p vsilyaev
 * PR 52171: Use special sequence to get the video PTS (b_play_getpts
 *  could return either audio or video PTS)
 *
 * 29   2/26/09 9:03a erickson
 * PR52099: block all TrickState calls to AudioDecoder and VideoDecoder if
 *  NEXUS_PlaybackHostTrickMode_eNormal is set. also removed some old,
 *  unused code.
 *
 * 28   2/25/09 11:19a katrep
 * PR52099: Need to take into account decoder trick modes. Audio needs to
 *  be muted.
 *
 * 27   2/18/09 4:43p katrep
 * PR51989: Fiorcing strict error check,reverting previous checkin. If
 *  audio decode has an error dont allow playback to continue.
 *
 * 26   2/10/09 12:00p katrep
 * PR51989: Do not stop video playback if audio decode fails for any
 *  reason.
 *
 * 25   1/27/09 4:04p erickson
 * PR51377: NEXUS_Playback_Start or NEXUS_Playback_OpenPidChannel should
 *  reset VideoDecoder's trick state
 *
 * 24   1/26/09 11:38a vsilyaev
 * PR 51579: Added ES to PES packetization and support for capturing of
 *  streams produced by the playback module
 *
 * 23   1/9/09 4:53p jgarrett
 * PR 49714: Fixing audio STC pause
 *
 * 22   1/9/09 1:55a erickson
 * PR49294: clean up WRN message
 *
 * 21   12/18/08 3:43p erickson
 * PR49605: fix playpump rate when leaving host paced trick mode to 1.2x
 *
 * 20   12/18/08 3:25p erickson
 * PR50541: fix vdecState.hostTrickModesEnabled assignment
 *
 * 19   12/16/08 7:09a erickson
 * PR49930: init everything in b_play_trick_init
 *
 * 18   12/10/08 11:05a erickson
 * PR49930: added NEXUS_Playback_AccurateSeekInProcess test api, added
 *  required flush before calling NEXUS_VideoDecoder_SetStartPts
 *
 * 17   11/19/08 1:30p erickson
 * PR47758: NEXUS_VideoDecoder_GetNextPts may fail on transitions. don't
 *  propogate failure to app. next call should work.
 *
 * 16   10/23/08 4:21p jtna
 * PR47758: NEXUS_VideoDecoder_GetNextPts() now returns nextPTS, not PTS
 *  increment
 *
 * 15   10/16/08 3:38p erickson
 * PR47673: mute decoder for STC trick modes other than 0.8 and 1.2
 *
 * 14   10/14/08 3:36p erickson
 * PR47407: reapply audio decoder trick mode settings when pid channel is
 *  opened or set
 *
 * 13   10/13/08 10:44a erickson
 * PR47572: downgrade DBG to MSG
 *
 * 12   10/9/08 5:30p erickson
 * PR47758: added temp workaround. also added one more stc trick state
 *  fix.
 *
 * 11   10/9/08 5:19p erickson
 * PR47608: must set decoder trick state even when in stc trick mode
 *
 * 10   10/9/08 2:54p erickson
 * PR47608: move location of stcChannel test
 *
 * 9   10/9/08 2:23p erickson
 * PR47608: NEXUS_PlaybackSettings.stcChannel is now required for TSM
 *  playback. NEXUS_PlaybackSettings.stcTrick was added for stc trick
 *  modes.
 *
 * 8   10/6/08 11:48a gmohile
 * PR 47608 : Invalidate STC channel during flush
 *
 * 7   10/1/08 4:43p jtna
 * PR37222: Use NEXUS_VideoDecoder_GetNextPts for STC trickmode frame
 *  advance
 *
 * 6   9/23/08 7:16p katrep
 * PR45880: Fixed 80% STC increment/prescal ratio for correct 80% STC
 *  clock
 *
 * 5   9/18/08 6:28p katrep
 * PR45880: Add support for FSPB(80/120 playback)
 *
 * 4   5/30/08 2:56p erickson
 * PR43000: improve BDBG_MSG
 *
 * 3   5/22/08 8:28p jgarrett
 * PR 42974: Adding audio mute flag
 *
 * 2   5/5/08 4:46p vsilyaev
 * PR 42355: Fixed rewind for AVI files
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/15   1/8/08 2:23p vsilyaev
 * PR 35824: Fixed resync on transition between modes
 *
 * Nexus_Devel/14   12/20/07 5:24p vsilyaev
 * PR 38073: Improved handling of trickmodes
 *
 * Nexus_Devel/13   12/20/07 3:41p vsilyaev
 * PR 38073: Improving use of bmedia_player
 *
 * Nexus_Devel/12   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 *
 * Nexus_Devel/PR38073/1   12/19/07 5:01p vsilyaev
 * PR 38073: Moved time tracking code for host paced mode into the
 * bmedia_player
 *
 * Nexus_Devel/11   12/4/07 2:12p vsilyaev
 * PR 36404: Use structure to conrol audio trickmodes
 *
 * Nexus_Devel/10   12/4/07 10:23a erickson
 * PR35825: fix warning
 *
 * Nexus_Devel/9   12/3/07 4:42p erickson
 * PR35825: restore normal rate STC on shutdown. keep video_decoder in TSM
 * when doing STC trick modes
 *
 * Nexus_Devel/8   11/16/07 12:38p vsilyaev
 * PR 35824: Added STC trickmodes
 *
 * Nexus_Devel/7   10/30/07 5:05p vsilyaev
 * PR 36404: Added audio tracking for slow motion modes
 *
 * Nexus_Devel/6   10/24/07 5:40p vsilyaev
 * PR 36404: Added audio decode trick mode
 *
 * Nexus_Devel/5   10/24/07 1:19p vsilyaev
 * PR 36404: Added audio flush
 *
 * Nexus_Devel/4   10/19/07 11:21a vsilyaev
 * PR 35824: Fixed data corruption on trick modes
 *
 * Nexus_Devel/3   10/17/07 5:38p vsilyaev
 * PR 35824: Added trickmode test
 *
 * Nexus_Devel/2   10/17/07 1:05p vsilyaev
 * PR 35824: Added playpump_trick
 *
 * Nexus_Devel/1   10/16/07 12:56p vsilyaev
 * PR 35824: Splitting playback into managable piexies
 *
 * $copied_brcm_Log: /BSEAV/api/src/pvr/bsettop_playback.c $
 * $copied_brcm_Revision: 172 $
 * $copied_brcm_Date: 10/1/07 11:13a $
 *
 * $copied_brcm_Log: /BSEAV/api/src/7401/bsettop_playpump_trick.c $
 * $copied_brcm_Revision: 19 $
 * $copied_brcm_Date: 9/17/07 9:15a $
 **************************************************************************/
#include "nexus_playback_module.h"
#include "nexus_playback_impl.h"
#include "nexus_audio_decoder_trick.h"

BDBG_MODULE(nexus_playback_trick);

static void b_play_trick_stc_normal(NEXUS_PlaybackHandle p);

static void
b_play_trick_monitor(void *p_)
{
    NEXUS_PlaybackHandle p = p_;
    BERR_Code rc;
    uint32_t video_pts;
    const NEXUS_Playback_P_PidChannel *pid;
    const NEXUS_Playback_P_PidChannel *decode;
    NEXUS_VideoDecoderStatus status;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);
    p->trick.rap_monitor_timer = NULL;

    decode = b_play_get_video_decoder(p);
    if(!decode) { goto done; }
    rc = NEXUS_P_Playback_VideoDecoder_GetStatus(decode, &status);
    if(rc!=NEXUS_SUCCESS) {goto done;}
    if(status.ptsType == NEXUS_PtsType_eCoded || status.ptsType == NEXUS_PtsType_eInterpolatedFromValidPTS || p->params.playpumpSettings.transportType==NEXUS_TransportType_eEs) {
        video_pts = status.pts;
    } else {goto done; }
    BDBG_MSG(("b_play_trick_monitor: %#x, video pts %#x", (unsigned)p, video_pts));

    for(pid = BLST_S_FIRST(&p->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        if(pid->cfg.pidSettings.pidType==NEXUS_PidType_eAudio) {
            rc = NEXUS_Playback_P_AudioDecoder_Advance(pid, video_pts);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
        }
    }

done: /* keep going anyway */
    p->trick.rap_monitor_timer = NEXUS_ScheduleTimer(100, b_play_trick_monitor, p);
    return;
}

void
b_play_flush(NEXUS_PlaybackHandle playback)
{
    NEXUS_Error rc;
    const NEXUS_Playback_P_PidChannel *pid;

    BDBG_MSG(("b_play_flush:> %#lx", (unsigned long)playback));

    playback->state.decoder_flushed = true;
    BDBG_ASSERT(playback->params.playpump);
    rc = NEXUS_Playpump_Flush(playback->params.playpump);
    if(rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc);}

    if(playback->params.stcChannel) {
        rc = NEXUS_StcChannel_Invalidate(playback->params.stcChannel);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc);}
    }

    for(pid = BLST_S_FIRST(&playback->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        switch(pid->cfg.pidSettings.pidType) {
        default: break;
        case NEXUS_PidType_eVideo: NEXUS_P_Playback_VideoDecoder_Flush(pid); break;
        case NEXUS_PidType_eAudio: NEXUS_P_Playback_AudioDecoder_Flush(pid); break;
        }
    }
#if B_PLAYBACK_CAPTURE
    if(playback->capture.has_data) {
        b_play_capture_close(playback);
        b_play_capture_open(playback);
    }
#endif

    BDBG_MSG(("b_play_flush:< %#lx", (unsigned long)playback));
    return;
}

void
b_play_trick_init(NEXUS_PlaybackHandle p)
{
    BKNI_Memset(&p->trick.settings, 0, sizeof(p->trick.settings));
    p->trick.settings.state = b_trick_state_normal;
    p->trick.settings.forward = true;
    p->trick.settings.decode_rate = NEXUS_NORMAL_PLAY_SPEED;
    p->trick.settings.decode_mode = NEXUS_VideoDecoderDecodeMode_eAll;
    p->trick.settings.fragmented = false;
    p->trick.settings.reordering_mode = NEXUS_VideoDecoderReorderingMode_eNone;
    p->trick.rap_monitor_timer = NULL;
    return;
}

void
b_play_trick_shutdown(NEXUS_PlaybackHandle p)
{
    if (p->trick.rap_monitor_timer) {
        NEXUS_CancelTimer(p->trick.rap_monitor_timer);
        p->trick.rap_monitor_timer = NULL;
    }

    b_play_trick_stc_normal(p);
    return;
}

void
b_play_trick_get(NEXUS_PlaybackHandle p, b_trick_settings *cfg)
{
    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);
    *cfg = p->trick.settings;
}

static NEXUS_Error
b_play_trick_set_each_audio(NEXUS_PlaybackHandle p, const NEXUS_Playback_P_PidChannel *pid, const b_trick_settings *settings)
{
    NEXUS_Error rc;
    NEXUS_AudioDecoderTrickState audioState;
    NEXUS_AudioDecoderStatus status;

    rc = NEXUS_P_Playback_AudioDecoder_GetStatus(pid, &status, NULL);
    if (rc) return BERR_TRACE(rc);
    if (!status.started) {
        BDBG_MSG(("Playback unable to set AudioDecoder trick state because it has not been started. Playback will continue, but you may not get desired results."));
        /* TODO: consider a callback from VideoDecoder to apply Playback's trick state when it is started */
        return 0;
    }

    NEXUS_P_Playback_AudioDecoder_GetTrickState(pid, &audioState, NULL);

    if(!settings->stc_trick) {
        /* decoder trick modes */
        if(settings->decode_rate < NEXUS_NORMAL_PLAY_SPEED) {
            audioState.rate = 0;
        }
        else {
            audioState.rate = settings->decode_rate;
        }
    }
    else {
        /* stc trick modes */
        if (settings->decode_rate != 0) {
            /* for non-0, we pass down the rate for DSOLA trick modes. */
            audioState.rate = settings->decode_rate;
        }
        else {
            /* if 0, just do an STC pause, no RAP pause. but we also have to return to mute. */
            audioState.rate = NEXUS_NORMAL_PLAY_SPEED;
        }
    }
    if (settings->audio_only_pause) {
        audioState.rate = 0;
    }

    audioState.forceStopped = settings->state != b_trick_state_normal;
    if(settings->decode_mode!=NEXUS_VideoDecoderDecodeMode_eAll) {
        audioState.forceStopped = true;
    }

    if(audioState.forceStopped) {
        audioState.rate = NEXUS_NORMAL_PLAY_SPEED;
    }

    /* mute for stc based pause */
    if ((settings->stc_trick && settings->decode_rate == 0)) {
        audioState.muted = true;
    }
    else {
        audioState.muted = false;
    }

    BDBG_MSG(("audio trick muted=%d, rate=%d, forceStopped=%d", audioState.muted, audioState.rate, audioState.forceStopped));

    rc = NEXUS_P_Playback_AudioDecoder_SetTrickState(pid, &audioState);
    if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto error;}

    if((audioState.rate==0 || settings->audio_only_pause) && !settings->stc_trick) {
        if(!p->trick.rap_monitor_timer) {
            p->trick.rap_monitor_timer = NEXUS_ScheduleTimer(100, b_play_trick_monitor, p);
        }
    } else {
        if (p->trick.rap_monitor_timer) {
            NEXUS_CancelTimer(p->trick.rap_monitor_timer);
            p->trick.rap_monitor_timer = NULL;
        }
    }

    return NEXUS_SUCCESS;

error:
    return rc;
}

void NEXUS_Playback_P_VideoDecoderFirstPts(void *context)
{
    NEXUS_PlaybackHandle playback = context;

    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);

    if (playback->state.inAccurateSeek) {
        b_trick_settings settings;
        /* video has found its first pts, so now audio can chase */
        /* NOTE: Due to XVD callback order, FirstPtsPassed cback can occur before FirstPts */
        /*       So check if we are in audio pause mode */
        b_play_trick_get(playback, &settings);
        if( settings.audio_only_pause ) {
           /* We got FirstPtsPassed first ! */
           settings.audio_only_pause = false;
           b_play_trick_set(playback, &settings);
           playback->state.inAccurateSeek = false;
           BDBG_WRN(("accurate seek: first pts passed, audio seek done FirstPts"));
        }
        else {
            BDBG_WRN(("accurate seek: first pts, starting audio seek FirstPts"));
            settings.audio_only_pause = true;
        }
        b_play_trick_set(playback, &settings);
    }
    return;
}

void NEXUS_Playback_P_VideoDecoderFirstPtsPassed(void *context)
{
    NEXUS_PlaybackHandle playback = context;

    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);

    if (playback->state.inAccurateSeek) {
        b_trick_settings settings;
        b_play_trick_get(playback, &settings);
        if( settings.audio_only_pause ) {
            /* we're done. video has arrived. now audio can start. */
            settings.audio_only_pause = false;
            b_play_trick_set(playback, &settings);
            playback->state.inAccurateSeek = false;
            BDBG_WRN(("accurate seek: first pts passed, audio seek done PtsPassed"));
        }
        else {
            BDBG_WRN(("accurate seek: first pts, starting audio seek PtsPassed"));
            settings.audio_only_pause = true;
            b_play_trick_set(playback, &settings);
        }
    }
    return;
}

NEXUS_Error
b_play_trick_set_pid(NEXUS_PlaybackHandle p, const NEXUS_Playback_P_PidChannel *pid, const b_trick_settings *settings)
{
    NEXUS_Error rc;
    bool slowMotion;

    if (!settings) {
        /* reapply previous settings */
        settings = &p->trick.settings;
    }

    switch (pid->cfg.pidSettings.pidType) {
    case NEXUS_PidType_eVideo:
        {
        NEXUS_VideoDecoderTrickState vdecState;
        NEXUS_VideoDecoderStatus status;
        NEXUS_VideoDecoderPlaybackSettings playbackSettings;

        if (!pid->cfg.pidTypeSettings.video.decoder && !pid->cfg.pidTypeSettings.video.simpleDecoder) {
            /* no decoder can be normal. already checked at NEXUS_Playback_TrickMode. */
            return 0;
        }

        NEXUS_P_Playback_VideoDecoder_GetPlaybackSettings(pid, &playbackSettings);
        if( playbackSettings.firstPts.callback == NULL ) {
            NEXUS_CallbackHandler_PrepareCallback(p->videoDecoderFirstPts, playbackSettings.firstPts);
            NEXUS_CallbackHandler_PrepareCallback(p->videoDecoderFirstPtsPassed, playbackSettings.firstPtsPassed);
            rc = NEXUS_P_Playback_VideoDecoder_SetPlaybackSettings(pid, &playbackSettings);
            if (rc) return BERR_TRACE(rc);
        }

        rc = NEXUS_P_Playback_VideoDecoder_GetStatus(pid, &status);
        if (rc) return BERR_TRACE(rc);
        if (status.tsm && !p->params.stcChannel && pid->cfg.pidTypeSettings.video.decoder) {
            /* This was a recent change. Apps must be fixed. Without this, certain trick mode transitions will fail
            because Playback will be unable to invalidate the STC. */
            BDBG_ERR(("Playback with TSM requires NEXUS_PlaybackSettings.stcChannel."));
            return BERR_TRACE(NEXUS_UNKNOWN);
        }
        if (!status.started) {
            BDBG_MSG(("Playback unable to set VideoDecoder trick state because it has not been started. Playback will continue, but you may not get desired results."));
            /* TODO: consider a callback from VideoDecoder to apply Playback's trick state when it is started */
            return 0;
        }

        NEXUS_P_Playback_VideoDecoder_GetTrickState(pid, &vdecState);
        if((settings->decode_rate!=0 && settings->state == b_trick_state_host_paced) || (settings->stc_trick && settings->state == b_trick_state_normal)) {
            vdecState.rate = NEXUS_NORMAL_PLAY_SPEED;
        } else {
            vdecState.rate = settings->decode_rate;
        }
        vdecState.reorderingMode = settings->reordering_mode;
        vdecState.decodeMode = settings->decode_mode;
        vdecState.brcmTrickModesEnabled = settings->state == b_trick_state_brcm_trick_mode;
        vdecState.hostTrickModesEnabled = settings->state == b_trick_state_host_paced || settings->state == b_trick_state_host_trick_mode;
        vdecState.dqtEnabled = settings->state == b_trick_state_display_queue_trick_mode;
        slowMotion = (-NEXUS_NORMAL_PLAY_SPEED < (int)settings->decode_rate && settings->decode_rate < NEXUS_NORMAL_PLAY_SPEED);
        vdecState.topFieldOnly = slowMotion || settings->state == b_trick_state_host_paced; /* show only top field if decoder expected to repeat frames, this is for slow motion or when using host paced trickmodes */
        vdecState.reverseFields = !settings->forward;
        if (settings->simulated_tsm) {
            vdecState.tsmEnabled = NEXUS_TsmMode_eSimulated;
            vdecState.rate = settings->decode_rate;
        }
        else {
            vdecState.tsmEnabled = (settings->decode_rate!=0 && settings->state == b_trick_state_host_paced) ||
                (settings->state == b_trick_state_normal &&
                 ((settings->decode_rate >= NEXUS_NORMAL_PLAY_SPEED && settings->decode_mode==NEXUS_VideoDecoderDecodeMode_eAll)
                  || settings->stc_trick)
                );
        }
        vdecState.forceSourceFrameRate = settings->force_source_frame_rate;
        vdecState.maxFrameRepeat = settings->maxFrameRepeat;
        BDBG_MSG(("calling NEXUS_VideoDecoder_SetTrickState: rate=%d %s %s %s %s %s %s %s", (int)vdecState.rate, B_DECODE_MODE_STR(vdecState.decodeMode),
            vdecState.brcmTrickModesEnabled?"brcm-trick":"", vdecState.hostTrickModesEnabled?"host-trick":"", vdecState.dqtEnabled?"dqt":"",
            vdecState.topFieldOnly?"top-field-only":"", vdecState.reverseFields?"reverse-field":"", vdecState.tsmEnabled?"tsm":"vsync"));
        rc = NEXUS_P_Playback_VideoDecoder_SetTrickState(pid, &vdecState);
        if (rc) return BERR_TRACE(rc);
        }
        break;

    case NEXUS_PidType_eAudio:
        rc = b_play_trick_set_each_audio(p, pid, settings);
        if (rc)  return BERR_TRACE(rc);
        break;

    default:
        break;
    }

    return 0;
}

static void remove_common_factors(unsigned val1, unsigned val2, unsigned *pResult1, unsigned *pResult2)
{
    static const unsigned factors[] = {5, 3};
    static const unsigned numFactors = sizeof(factors)/sizeof(unsigned);
    unsigned factor, index;

    /* Remove factors of 2 first to use AND/shift operations instead of div/mod */
    while ( val1 >= 2 && val2 >= 2 )
    {
        if ( (val1 & 1) || (val2 & 1) )
        {
            break;
        }

        val1 >>= 1;
        val2 >>= 1;
    }

    /* Now go through remaining factors */
    for ( index = 0; index < numFactors; index++ )
    {
        factor = factors[index];
        while ( val1 >= factor && val2 >= factor )
        {
            if ( (val1 % factor) || (val2 % factor) )
            {
                break;
            }

            val1 /= factor;
            val2 /= factor;
        }
    }

    *pResult1 = val1;
    *pResult2 = val2;
}


static NEXUS_Error
b_play_trick_stc_set_rate(NEXUS_PlaybackHandle p, unsigned decode_rate)
{
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_Error rc;
    unsigned denominator,numerator;

    BDBG_MSG(("b_play_trick_stc_set_rate %d", decode_rate));

    if (decode_rate != NEXUS_NORMAL_PLAY_SPEED && !b_play_decoders_in_tsm_mode(p)) {
        BDBG_ERR(("STC trick modes require decoder(s) to be in TSM mode"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    stcChannel = p->params.stcChannel;
    if (!stcChannel) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    remove_common_factors( decode_rate, NEXUS_NORMAL_PLAY_SPEED, &numerator, &denominator);
    if ( numerator && (numerator > 255 || denominator > 255) ) {
        /* This will occur for prime number numerators between min decode rate and max decode rate, eg 1009/1000 */
        BDBG_MSG(("common Factors > 255, stcTrickmode decode rate not optimal numr=%d denr=%d!", numerator, denominator ));
    }

    rc = NEXUS_StcChannel_SetRate(stcChannel, numerator, denominator - 1 /* -1 because HW uses denom+1 for prescale val */ );
    if(rc!=NEXUS_SUCCESS) return BERR_TRACE(rc);

    return NEXUS_SUCCESS;
}

static void
b_play_trick_stc_normal(NEXUS_PlaybackHandle p)
{
    if (p->params.stcChannel) {
        (void)NEXUS_StcChannel_SetRate(p->params.stcChannel, 1, 0 /* HW adds one */ );
    }
    return;
}

NEXUS_Error
b_play_trick_set(NEXUS_PlaybackHandle p, const b_trick_settings *settings)
{
    NEXUS_Error rc;
    const NEXUS_Playback_P_PidChannel *pid;

    BDBG_CASSERT(NEXUS_NORMAL_PLAY_SPEED == NEXUS_NORMAL_DECODE_RATE);

    if (!settings) {
        /* reapply previous settings */
        settings = &p->trick.settings;
    }

    BDBG_MSG(("b_play_trick_set: %#lx %s->%s %s->%s %d->%d %d->%d %s->%s", (unsigned long)p,
        B_TRICK_STATE_STR(p->trick.settings.state), B_TRICK_STATE_STR(settings->state),
        B_FORWARD_STR(p->trick.settings.forward), B_FORWARD_STR(settings->forward),
        p->trick.settings.decode_rate, settings->decode_rate,
        p->trick.settings.stc_trick, settings->stc_trick,
        B_DECODE_MODE_STR(p->trick.settings.decode_mode), B_DECODE_MODE_STR(settings->decode_mode) ));

    /* fail invalid trick modes */
    if (settings->state == b_trick_state_normal && !settings->forward) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_settings;
    }

    /* if the state and direction aren't changing, we don't need to flush. just jump over. */
    if (p->trick.settings.state == settings->state &&
        p->trick.settings.forward == settings->forward &&
        (p->trick.settings.decode_mode == settings->decode_mode || p->trick.settings.avoid_flush))
    {
        goto set_decoder_state;
    }

    /* this flushes both playback and decoders. this must occur before changing decoder state.
    if not, the new decoder state might be applied to a small amount of the old data and undesired
    results may be seen. if the interface to the decoder is slow (which it is), the likelihood
    increases. */
    b_play_flush(p);

set_decoder_state:

    if(p->trick.settings.state==b_trick_state_host_paced  || settings->state == b_trick_state_host_paced) {
        NEXUS_PlaypumpSettings pumpCfg;

        NEXUS_Playpump_GetSettings(p->params.playpump, &pumpCfg);
        BDBG_CASSERT(NEXUS_NORMAL_DECODE_RATE==NEXUS_NORMAL_PLAY_SPEED);
        if(settings->decode_rate!=0 && settings->state == b_trick_state_host_paced) {
            pumpCfg.playRate = settings->decode_rate;
        } else {
            if(settings->forward) {
                pumpCfg.playRate = NEXUS_NORMAL_DECODE_RATE;
            } else {
                pumpCfg.playRate = -NEXUS_NORMAL_DECODE_RATE;
            }
        }
        rc = NEXUS_Playpump_SetSettings(p->params.playpump, &pumpCfg);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_settings;}
    }

    /* SW35230-2125, invalidate STC channel to force the STC to get reloaded. This is
    required to minimize the lost of audio/video data when transitioning from
    decoder_pause-->play or decoder_pause->seek->play.  The problem shows up if the
    time period between transition is long. */
    if(!settings->stc_trick && p->params.stcChannel) {
        rc = NEXUS_StcChannel_Invalidate(p->params.stcChannel);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_settings;}
    }

    for (pid = BLST_S_FIRST(&p->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        rc = b_play_trick_set_pid(p, pid, settings);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_set_pid;}
    }

    /* decoders must already be set into their state before setting STC */
    if (settings->stc_trick && settings->state == b_trick_state_normal) {
        /* set stc trick mode rate */
        rc = b_play_trick_stc_set_rate(p, settings->decode_rate);
        if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_settings;}
    }
    else {
        /* if playback has the stcChannel, set it to a normal rate in all other cases. */
        b_play_trick_stc_normal(p);
    }

    p->trick.settings = *settings;
    return NEXUS_SUCCESS;

err_set_pid:
err_settings:
    return rc;
}

NEXUS_Error
b_play_trick_frameadvance(NEXUS_PlaybackHandle p)
{
    const NEXUS_Playback_P_PidChannel *pid;
    NEXUS_Error rc;

    pid = b_play_get_video_decoder(p);
    if (!pid) {
        BDBG_WRN(("Can't do frame advance without Nexus_VideoDecoderHandle"));
        BDBG_WRN(("Please set NEXUS_PlaybackPidChannelSettings.pidTypeSettings.video.decoder before calling NEXUS_Playback_OpenPidChannel()"));
        rc = BERR_TRACE(BERR_INVALID_PARAMETER);
        goto err_advance;
    }

    if (pid->cfg.pidTypeSettings.video.decoder) {
        if (p->trick.settings.stc_trick && p->trick.settings.forward) {
            uint32_t next_pts, stc;
            NEXUS_StcChannelSettings stcSettings;

            NEXUS_StcChannel_GetStc(p->params.stcChannel, &stc);
            rc = NEXUS_VideoDecoder_GetNextPts(pid->cfg.pidTypeSettings.video.decoder, &next_pts);
            if (rc!=NEXUS_SUCCESS) {
                BDBG_WRN(("cannot frame advance. PTS not available yet."));
                /* don't fail. this is normal on transition. app should simply call again. */
                return BERR_SUCCESS;
            }
            NEXUS_StcChannel_GetSettings(p->params.stcChannel, &stcSettings);

            BDBG_MSG(("b_play_trick_frameadvance: STC=%08X, Next PTS=%08X", stc, next_pts));
            stc = next_pts + stcSettings.modeSettings.Auto.offsetThreshold;
            rc = NEXUS_StcChannel_SetStc(p->params.stcChannel, stc);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto err_advance;}
        }
        else {
            BDBG_MSG(("b_play_trick_frameadvance: video frame advance"));
            rc = NEXUS_VideoDecoder_FrameAdvance(pid->cfg.pidTypeSettings.video.decoder);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_advance;}
        }
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.video.simpleDecoder) {
        BDBG_MSG(("b_play_trick_frameadvance: video frame advance"));
        rc = NEXUS_SimpleVideoDecoder_FrameAdvance(pid->cfg.pidTypeSettings.video.simpleDecoder);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_advance;}
    }
#endif

    return NEXUS_SUCCESS;
err_advance:
    return rc;
}

NEXUS_Error NEXUS_P_Playback_VideoDecoder_GetStatus(const NEXUS_Playback_P_PidChannel *pid, NEXUS_VideoDecoderStatus *pStatus)
{
    if (pid->cfg.pidTypeSettings.video.decoder) {
        return NEXUS_VideoDecoder_GetStatus(pid->cfg.pidTypeSettings.video.decoder, pStatus);
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.video.simpleDecoder) {
        return NEXUS_SimpleVideoDecoder_GetStatus(pid->cfg.pidTypeSettings.video.simpleDecoder, pStatus);
    }
#endif
    return NEXUS_NOT_AVAILABLE; /* no BERR_TRACE. could be normal. */
}

void NEXUS_P_Playback_VideoDecoder_Flush(const NEXUS_Playback_P_PidChannel *pid)
{
    /* only apply to one. there will never be >1 at a time. */
    if(pid->cfg.pidTypeSettings.video.decoder) {
        NEXUS_VideoDecoder_Flush(pid->cfg.pidTypeSettings.video.decoder);
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if(pid->cfg.pidTypeSettings.video.simpleDecoder) {
        NEXUS_SimpleVideoDecoder_Flush(pid->cfg.pidTypeSettings.video.simpleDecoder);
    }
#endif
}

void NEXUS_P_Playback_VideoDecoder_GetTrickState(const NEXUS_Playback_P_PidChannel *pid, NEXUS_VideoDecoderTrickState *pState)
{
    if (pid->cfg.pidTypeSettings.video.decoder) {
        NEXUS_VideoDecoder_GetTrickState(pid->cfg.pidTypeSettings.video.decoder, pState);
        return;
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.video.simpleDecoder) {
        NEXUS_SimpleVideoDecoder_GetTrickState(pid->cfg.pidTypeSettings.video.simpleDecoder, pState);
        return;
    }
#endif
    BKNI_Memset(pState, 0, sizeof(*pState));
    return;
}

NEXUS_Error NEXUS_P_Playback_VideoDecoder_SetTrickState(const NEXUS_Playback_P_PidChannel *pid, const NEXUS_VideoDecoderTrickState *pState)
{
    /* only apply to one. there will never be >1 at a time. */
    if (pid->cfg.pidTypeSettings.video.decoder) {
        return NEXUS_VideoDecoder_SetTrickState(pid->cfg.pidTypeSettings.video.decoder, pState);
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.video.simpleDecoder) {
        return NEXUS_SimpleVideoDecoder_SetTrickState(pid->cfg.pidTypeSettings.video.simpleDecoder, pState);
    }
#endif
    return BERR_TRACE(NEXUS_NOT_AVAILABLE);
}

void NEXUS_P_Playback_VideoDecoder_GetPlaybackSettings(const NEXUS_Playback_P_PidChannel *pid, NEXUS_VideoDecoderPlaybackSettings *pSettings)
{
    if (pid->cfg.pidTypeSettings.video.decoder) {
        NEXUS_VideoDecoder_GetPlaybackSettings(pid->cfg.pidTypeSettings.video.decoder, pSettings);
        return;
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.video.simpleDecoder) {
        NEXUS_SimpleVideoDecoder_GetPlaybackSettings(pid->cfg.pidTypeSettings.video.simpleDecoder, pSettings);
        return;
    }
#endif
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

NEXUS_Error NEXUS_P_Playback_VideoDecoder_SetPlaybackSettings(const NEXUS_Playback_P_PidChannel *pid, const NEXUS_VideoDecoderPlaybackSettings *pSettings)
{
    /* only apply to one. there will never be >1 at a time. */
    if (pid->cfg.pidTypeSettings.video.decoder) {
        return NEXUS_VideoDecoder_SetPlaybackSettings(pid->cfg.pidTypeSettings.video.decoder, pSettings);
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.video.simpleDecoder) {
        return NEXUS_SimpleVideoDecoder_SetPlaybackSettings(pid->cfg.pidTypeSettings.video.simpleDecoder, pSettings);
    }
#endif
    return BERR_TRACE(NEXUS_NOT_AVAILABLE);
}

NEXUS_Error NEXUS_P_Playback_AudioDecoder_GetStatus(const NEXUS_Playback_P_PidChannel *pid, NEXUS_AudioDecoderStatus *pStatus, NEXUS_AudioDecoderStatus *pSecondaryStatus)
{
    NEXUS_Error rc;

    /* primary and simple are mutually exclusive for status */
    if (pid->cfg.pidTypeSettings.audio.primary) {
        rc = NEXUS_AudioDecoder_GetStatus(pid->cfg.pidTypeSettings.audio.primary, pStatus);
        if (rc) return BERR_TRACE(rc);
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.audio.simpleDecoder) {
        rc = NEXUS_SimpleAudioDecoder_GetStatus(pid->cfg.pidTypeSettings.audio.simpleDecoder, pStatus);
        if (rc) return BERR_TRACE(rc);
    }
#endif
    else {
        BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    }

    if (pSecondaryStatus) {
        if (pid->cfg.pidTypeSettings.audio.secondary) {
            rc = NEXUS_AudioDecoder_GetStatus(pid->cfg.pidTypeSettings.audio.secondary, pSecondaryStatus);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            BKNI_Memset(pSecondaryStatus, 0, sizeof(*pSecondaryStatus));
        }
    }
    /* if there are no decoders for this pid, status will be memset(0), which translates to status.started = false. */
    return 0;
}

void NEXUS_P_Playback_AudioDecoder_Flush(const NEXUS_Playback_P_PidChannel *pid)
{
    NEXUS_Error rc;
    if(pid->cfg.pidTypeSettings.audio.primary) {
        rc = NEXUS_AudioDecoder_Flush(pid->cfg.pidTypeSettings.audio.primary);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    }
    if(pid->cfg.pidTypeSettings.audio.secondary) {
        rc = NEXUS_AudioDecoder_Flush(pid->cfg.pidTypeSettings.audio.secondary);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    }
#if NEXUS_HAS_SIMPLE_DECODER
    if (pid->cfg.pidTypeSettings.audio.simpleDecoder) {
        NEXUS_SimpleAudioDecoder_Flush(pid->cfg.pidTypeSettings.audio.simpleDecoder);
    }
#endif
    if (pid->cfg.pidTypeSettings.audio.primer) {
        NEXUS_AudioDecoderPrimer_Flush(pid->cfg.pidTypeSettings.audio.primer);
    }
}

NEXUS_Error NEXUS_Playback_P_AudioDecoder_Advance(const NEXUS_Playback_P_PidChannel *pid, uint32_t video_pts)
{
    NEXUS_Error rc;
    if(pid->cfg.pidTypeSettings.audio.primary) {
        rc = NEXUS_AudioDecoder_Advance(pid->cfg.pidTypeSettings.audio.primary, video_pts);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    }
    if(pid->cfg.pidTypeSettings.audio.secondary) {
        rc = NEXUS_AudioDecoder_Advance(pid->cfg.pidTypeSettings.audio.secondary, video_pts);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);}
    }
#if NEXUS_HAS_SIMPLE_DECODER
    if (pid->cfg.pidTypeSettings.audio.simpleDecoder) {
        rc = NEXUS_SimpleAudioDecoder_Advance(pid->cfg.pidTypeSettings.audio.simpleDecoder, video_pts);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);}
    }
#endif
    return 0;
}

void NEXUS_P_Playback_AudioDecoder_GetTrickState(const NEXUS_Playback_P_PidChannel *pid, NEXUS_AudioDecoderTrickState *pState, NEXUS_AudioDecoderTrickState *pSecondaryState)
{
    /* primary and simple are mutually exclusive for getting state */
    if (pid->cfg.pidTypeSettings.audio.primary) {
        NEXUS_AudioDecoder_GetTrickState(pid->cfg.pidTypeSettings.audio.primary, pState);
    }
#if NEXUS_HAS_SIMPLE_DECODER
    else if (pid->cfg.pidTypeSettings.audio.simpleDecoder) {
        NEXUS_SimpleAudioDecoder_GetTrickState(pid->cfg.pidTypeSettings.audio.simpleDecoder, pState);
    }
#endif
    else {
        BKNI_Memset(pState, 0, sizeof(*pState));
    }

    if (pSecondaryState) {
        if (pid->cfg.pidTypeSettings.audio.secondary) {
            NEXUS_AudioDecoder_GetTrickState(pid->cfg.pidTypeSettings.audio.secondary, pSecondaryState);
        }
        else {
            BKNI_Memset(pSecondaryState, 0, sizeof(*pSecondaryState));
        }
    }
    return;
}

NEXUS_Error NEXUS_P_Playback_AudioDecoder_SetTrickState(const NEXUS_Playback_P_PidChannel *pid, const NEXUS_AudioDecoderTrickState *pState)
{
    NEXUS_Error rc;
    /* set any available */
    if (pid->cfg.pidTypeSettings.audio.primary) {
        rc = NEXUS_AudioDecoder_SetTrickState(pid->cfg.pidTypeSettings.audio.primary, pState);
        if (rc) return BERR_TRACE(rc);
    }
    if (pid->cfg.pidTypeSettings.audio.secondary) {
        rc = NEXUS_AudioDecoder_SetTrickState(pid->cfg.pidTypeSettings.audio.secondary, pState);
        if (rc) return BERR_TRACE(rc);
    }
#if NEXUS_HAS_SIMPLE_DECODER
    if (pid->cfg.pidTypeSettings.audio.simpleDecoder) {
        rc = NEXUS_SimpleAudioDecoder_SetTrickState(pid->cfg.pidTypeSettings.audio.simpleDecoder, pState);
        if (rc) return BERR_TRACE(rc);
    }
#endif
    return 0;
}

