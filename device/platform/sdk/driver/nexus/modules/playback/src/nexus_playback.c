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
 * $brcm_Workfile: nexus_playback.c $
 * $brcm_Revision: 134 $
 * $brcm_Date: 10/22/12 9:21a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/playback/src/nexus_playback.c $
 * 
 * 134   10/22/12 9:21a erickson
 * SW7435-410: don't restart playback on frameadvance when already in
 *  drain mode
 * 
 * 133   10/15/12 10:38a erickson
 * SW7435-393: turn off simulated_tsm for normal play
 * 
 * 132   10/10/12 3:06p vsilyaev
 * SW7335-1212: Fixed merge
 * 
 * 131   10/10/12 2:59p vsilyaev
 * SW7335-1212: Merged  record deadlock
 * 
 * 130   10/8/12 4:38p erickson
 * SW7425-3594: fix crash in frameadvance with simulated_tsm mode
 * 
 * 129   9/4/12 4:46p erickson
 * SW7425-3594: support simulated_tsm mode from bmedia_player_nav
 * 
 * 128   7/30/12 11:02a erickson
 * SW7358-266: remove pidch ref_cnt from playpump/playback.
 *  NEXUS_Playpump_P_OpenPid will validate, which also allows
 *  pidChannelIndex options to be used.
 * 
 * 127   7/5/12 3:24p jgarrett
 * SW7425-2508: Not reporting 'Unable to look up position' BDBG_WRN when
 *  there is a decoder with an invalid pts
 * 
 * 126   6/15/12 11:51a erickson
 * SW7425-3081: add IsTransportTypeSupported
 * 
 * 125   6/6/12 12:41p vsilyaev
 * SW7425-1301: Improved handling of end of stream (and wrapping) during
 *  frame advance/reverse
 * 
 * 124   4/23/12 5:05p erickson
 * SWDEPRECATED-3645: only print one warning per start for missing decoder
 *  handles
 * 
 * 123   4/12/12 1:08p vsilyaev
 * SW7405-5645: Fixed typo
 * 
 * 122   2/14/12 3:34p erickson
 * SW7425-2355: restrict playpumpSettings changes in
 *  NEXUS_Playback_SetSettings to only those that are currently a problem
 * 
 * 121   2/6/12 1:22p erickson
 * SWDEPRECATED-3955: NEXUS_Playback_TrickMode should clear startPaused
 *  state
 * 
 * 120   1/2/12 11:27a erickson
 * SW7420-1965: return primary and secondary audio decoder status and
 *  trick state. make primary and simple decoder state mutually exclusive.
 *
 * 119   12/27/11 3:33p vsilyaev
 * SW7420-2198: Verify index type before doing accurate seek
 *
 * 118   12/20/11 1:43p vsilyaev
 * SW7425-2034: Used 32-bit type for program id (pid) types
 *
 * 117   12/9/11 10:07a erickson
 * SWDEPRECATED-1003: reset force_source_frame_rate on normal play
 *
 * 116   10/19/11 3:52p erickson
 * SW7335-1368: don't allow non-zero rate to round down to zero (pause)
 *
 * 115   10/12/11 11:44a erickson
 * SW7425-1264: convert decode_rate to abs before passing to
 *  nexus_playback_trick.c. if trick params must be adjusted, actually
 *  modify a copy of the struct for better debug.
 *
 * 114   10/12/11 9:32a erickson
 * SW7425-1264: improve param checking for automatic and custom trick
 *  modes
 *
 * 113   10/7/11 11:16a erickson
 * SW7125-823: removing check for video pid channel when starting
 *  timeshifting. valid usage modes exist where this is not true.
 *
 * 112   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 *
 * 111   9/7/11 11:41a erickson
 * SW7405-5059: fail stc trick if any decoder not in TSM mode
 *
 * 110   8/31/11 2:44p vsilyaev
 * SW7420-1978: Add selection of index type for playback of MPEG-2 TS
 *  files
 *
 * 109   8/30/11 12:46p vsilyaev
 * SWDTV-7867: Use proper scale factor when converting rate to time
 *
 * 108   8/12/11 9:40a erickson
 * SW7358-83: add NEXUS_PlaybackTrickModeSettings.brcmTrickMode
 *
 * 107   6/16/11 3:38p erickson
 * SW7125-823: if playback lacks decoder, allow NEXUS_Playback_Seek to
 *  succeed but issue a WRN
 *
 * 106   5/26/11 7:32p vsilyaev
 * SW7425-646: Added configurable 'gap' for timeshifting
 *
 * 105   5/13/11 5:12p erickson
 * SW7125-823: add support for decoder-less host trick modes with
 *  nexus_playback
 *
 * 104   5/13/11 4:13p erickson
 * SWDTV-6386: add "nx_" namespace prefix for internal nexus threads
 *
 * 103   4/27/11 10:34a erickson
 * SW7405-4249: all audio-only timeshifting
 *
 * 102   4/14/11 3:31p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 *
 * 101   3/28/11 3:45p vsilyaev
 * SW7335-1214: Added NEXUS_CallbackHandler framework
 *
 * 100   2/24/11 1:36p vsilyaev
 * SWDTV-5485: Differentiates ES and ES->PES types
 *
 * 99   11/24/10 3:10p vsilyaev
 * SW35230-2195: Better support of 'start_paused'->pause transition and
 *  cleaned error reporting when PTS can't be obtained
 *
 * 98   11/11/10 5:47p vsilyaev
 * SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
 *  GetDefaultSettings
 *
 * 97   11/2/10 7:10p mward
 * SW7400-2920: playback should not flush when transitioning decoder skip
 *  modes if there's no index.
 * 
 * 96   10/22/10 11:46a vsilyaev
 * SW35230-1600: When decrypting payload clear file buffer on transitions.
 * 
 * 95   9/28/10 1:57p erickson
 * SWDEPRECATED-1003: pass force source frame rate from media framework to
 *  VideoDecoder
 *
 * 93   6/29/10 3:20p erickson
 * SW7405-4367: playback should flush when transitioning decoder skip
 *  modes. also added NEXUS_PlaybackTrickModeSettings.avoidFlush option to
 *  disable this behavior.
 *
 * 92   6/1/10 3:07p vsilyaev
 * SW7400-2787: Improving handling of 'start_paused'
 *
 * 91   4/29/10 11:55a vsilyaev
 * SW7400-2749: If in 'start.paused' state don't start playback on
 *  GetStatus
 *
 * 90   4/14/10 5:43p jtna
 *  SW7125-317: Coverity Defect ID:20335 CHECKED_RETURN
 *
 * 89   3/11/10 5:26p vsilyaev
 * SW3556-913: Always use header file to declare external functions
 *
 * 88   3/5/10 7:12p jtna
 * SW3556-913: support for ErrorHandlingMode_eAbort
 *
 * 87   2/23/10 4:58p erickson
 * SW7400-2684: add state variable so we only fire beginningOfStreamAction
 *  or endOfStreamAction once when paused
 *
 * 86   2/22/10 6:55p vsilyaev
 * SW3556-913: Don't call  b_play_check_buffer before reading status
 *
 * 85   2/22/10 6:50p vsilyaev
 * SW3556-913: Return playback state even if in 'Aborted' or 'Paused'
 *  state
 *
 * 84   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 *
 * 83   1/13/10 5:41p vsilyaev
 * SW3556-913: Added option control error handling for Play/Seek/SetTrick
 *  operations
 *
 * 82   1/12/10 8:13p vsilyaev
 * SW3556-913: Don't sync pause with the playback data flow (file I/O)
 *
 * 81   12/17/09 6:16p vsilyaev
 * SW3556-913: Added option to specify timeout on asynchronous I/O
 *
 * 80   12/17/09 5:52p vsilyaev
 * SW3548-2677: Added configuration that allows user to choose what
 *  context should be used for synchronous I/O
 *
 * 79   11/16/09 10:31a vsilyaev
 * SW7405-3393 SW7400-2593: Use signed decode_rate for the host_paced mode
 *
 * 78   11/5/09 7:32p vsilyaev
 * SW7400-2593: Updated lookup table and handling of slow rewind modes
 *
 * 77   11/5/09 11:37a vsilyaev
 * SW7400-2593: Set trickModeSettings.rate to 0 while in the pause state
 *
 * 76   11/5/09 9:25a vsilyaev
 * SW7400-2593: Populate trickModeSettings inside NEXUS_Playback_GetStatus
 *
 * 75   10/14/09 10:24a jtna
 * SW7405-3205: fix typo in BDBG_ERR
 *
 * 74   9/30/09 7:12p vsilyaev
 * SW7405-2732: Fixed communication between media player and nexux
 *  playback when entering into the frame reverse mode
 *
 * 73   9/30/09 6:48p vsilyaev
 * SW7405-2732: Added fragmented mode, for the decoder side it set using
 *  Reoorder and filtering I frames only
 *
 * 72   9/9/09 12:04p erickson
 * SW7400-2507: NEXUS_Playback_CloseAllPidChannels should call
 *  NEXUS_Playback_ClosePidChannel so that cleanup logic doesn't have to
 *  be duplicated
 *
 * 71   8/19/09 3:25p erickson
 * PR57840: remove incorrect comment
 *
 * 70   8/10/09 3:36p ahulse
 * PR56762: firstPts/Passed callbacks not registered in settop API mode,
 *  move registration into b_play_trick_set_pid
 *
 * 69   7/31/09 4:29p erickson
 * PR57235: rename Settop API names, remove unused code
 *
 * 68   7/31/09 12:36p katrep
 * PR57193: stc_trick flag should be restored to requested value when
 *  resuming playback from trickmodes
 *
 * 67   7/30/09 7:15p vsilyaev
 * PR 55989: Do UnlockModule only after all playback componets where
 *  shutdown, this prevents from stray IO callback to reach into closed
 *  media player
 *
 * 66   7/21/09 5:57p vsilyaev
 * PR 56977: Nexus playback always control audio and video decoder.
 *  Provided way for the user to control the decoder slowmotion factor
 *  using the playback trick API.
 *
 * 65   7/13/09 3:51p jtna
 * PR56423: fix condition to set decoder trick state
 *
 * 64   7/8/09 3:25p vsilyaev
 * PR 55989: Used size of the video decoder CDB to control size of the
 *  data chunk
 *
 * 63   7/8/09 11:59a vsilyaev
 * PR 55989: Added support for OTF trickmodes
 *
 * 62   6/19/09 4:56p vsilyaev
 * PR 56169: Allow fast decode trickmodes even without STC trickmodes
 *
 * 61   6/19/09 4:11p vsilyaev
 * PR 56169: PR 53824: Allow user to sepcify max decodder rate. This
 *  superceeds overrideStcTrick.
 *
 * 60   6/11/09 4:31p jtna
 * PR55817: make check on duplicate channels backward-compatible
 *
 * 59   6/8/09 3:47p erickson
 * PR55817: relax the check on duplicate pid channels. they are allowed,
 *  as long as they are simultaneously enabled. we still prevent duplicate
 *  NEXUS_PidChannelHandle entries.
 *
 * 58   4/16/09 1:29p erickson
 * PR54129: failed Seek is not an ERR. prevent eWaitingRecord loop at
 *  beginning of circular record.
 *
 * 57   4/3/09 12:56p erickson
 * PR53824: reenable stc trick modes. add
 *  NEXUS_PlaybackTrickModeSettings.overrideStcTrick for additional app
 *  control.
 *
 * 56   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 *  combination with the position it could be used to measure depth of
 *  decoder and playback buffers in units of time.
 *
 * 55   3/6/09 1:55p erickson
 * PR52891: remove unnecessary call to b_play_trick_set_pid inside
 *  NEXUS_Playback_OpenPidChannel
 *
 * 54   3/5/09 5:46p vsilyaev
 * PR 52579: Updated circular FIFO/timeshifting code
 *
 * 53   2/26/09 9:03a erickson
 * PR52099: block all TrickState calls to AudioDecoder and VideoDecoder if
 *  NEXUS_PlaybackHostTrickMode_eNormal is set. also removed some old,
 *  unused code.
 *
 * 52   2/25/09 11:19a katrep
 * PR52099: Need to take into account decoder trick modes. Audio needs to
 *  be muted.
 *
 * 51   2/19/09 11:52a erickson
 * PR52290: Playback should fail on certain operations if there's no
 *  eAudio or eVideo pid set
 *
 * 50   2/13/09 11:49a erickson
 * PR52107: fix NEXUS_Playback_Seek when in startPaused state
 *
 * 49   2/5/09 4:48p erickson
 * PR51870: fix uninitialized variable
 *
 * 48   2/2/09 3:35p gmohile
 * PR 47815 : Add manual control of trickmodes
 *
 * 47   2/2/09 2:53p gmohile
 * PR 47815 : Add manual control of trickmodes
 *
 * 46   1/27/09 4:04p erickson
 * PR51377: NEXUS_Playback_Start or NEXUS_Playback_OpenPidChannel should
 *  reset VideoDecoder's trick state
 *
 * 45   1/26/09 11:38a vsilyaev
 * PR 51579: Added ES to PES packetization and support for capturing of
 *  streams produced by the playback module
 *
 * 44   1/12/09 5:45p vsilyaev
 * PR 50763: Improved seek to position 0 after rewind reached begining of
 *  the file
 *
 * 43   1/9/09 1:55a erickson
 * PR49294: don't allow NEXUS_PlaybackSettings.stcTrick to change while
 *  started
 *
 * 42   12/19/08 5:59p vsilyaev
 * PR 50214: Added callbacks and counters for parsing index files
 *
 * 41   12/15/08 2:41p erickson
 * PR50369: if b_play_trick_set fails, we can't abort all Playback state
 *  changes otherwise we end up with a stray NEXUS_Playpump_ReadComplete.
 *
 * 40   12/10/08 4:42p erickson
 * PR49930: add internal accurate seek algorithm for driving audio through
 *  the transition
 *
 * 39   12/10/08 11:11a erickson
 * PR49930: added NEXUS_Playback_AccurateSeekInProcess test api, added
 *  required flush before calling NEXUS_VideoDecoder_SetStartPts
 *
 * 38   12/10/08 2:06a vsilyaev
 * PR 48760: Using ES player for accurate progress monitor of MP3 streams
 *
 * 37   12/8/08 11:19a erickson
 * PR49930: implement NEXUS_PlaybackSettings.accurateSeek
 *
 * 36   11/21/08 12:06p erickson
 * PR49531: corrected comment
 *
 * 35   11/21/08 8:54a erickson
 * PR49531: cancel playback timer before stopping Playpump so that no
 *  additional calls to Playpump are made after stop
 *
 * 34   11/19/08 1:50p erickson
 * PR48744: don't require
 *  NEXUS_PlaybackPidChannelSettings.pidTypeSettings.video.index for
 *  decoder trick modes
 *
 * 33   11/18/08 4:58p erickson
 * PR47854: call NEXUS_FilePlay_Cancel if waiting for io times out. allow
 *  Playback to recover from eAborted state.
 *
 * 32   11/5/08 2:37p erickson
 * PR47407: fix warning
 *
 * 31   11/4/08 5:33p erickson
 * PR47407: remove call to b_play_trick_set and replace with direct
 *  assignment
 *
 * 30   10/23/08 3:52p erickson
 * PR48149: fix NEXUS_Playback_P_CheckWaitingIo state, don't force
 *  eStopped state
 *
 * 29   10/21/08 2:14p erickson
 * PR48149: implement algo to get out of eWaitingIo state
 *
 * 28   10/14/08 5:01p erickson
 * PR47407: only reapply audio decoder trick modes if already started
 *
 * 27   10/14/08 3:36p erickson
 * PR47407: reapply audio decoder trick mode settings when pid channel is
 *  opened or set
 *
 * 26   10/9/08 2:54p erickson
 * PR47608: move location of stcChannel test
 *
 * 25   10/9/08 2:23p erickson
 * PR47608: NEXUS_PlaybackSettings.stcChannel is now required for TSM
 *  playback. NEXUS_PlaybackSettings.stcTrick was added for stc trick
 *  modes.
 *
 * 24   10/6/08 11:48a gmohile
 * PR 47608 : Invalidate STC channel during flush
 *
 * 23   9/18/08 6:28p katrep
 * PR45880: Add support for FSPB(80/120 playback)
 *
 * 22   7/28/08 3:34p katrep
 * PR43242: Allow changes of timeshifting flag while playback is in
 *  progress
 *
 * 21   7/24/08 4:34p erickson
 * PR45124: allow duplicate calls to NEXUS_Playback_OpenPidChannel and
 *  ref_cnt internally
 *
 * 20   7/3/08 1:07p vsilyaev
 * PR 44381: Added code to limi nestendess of recursive calls
 *
 * 19   6/24/08 4:07p erickson
 * PR44107: rename to TrickMode
 *
 * 18   6/12/08 3:02p erickson
 * PR43606: rename NEXUS_PlaybackStreamSettings to
 *  NEXUS_PlaybackStartSettings
 *
 * 17   5/30/08 2:56p erickson
 * PR43000: fix default frame advance state at Start
 *
 * 16   5/22/08 7:25p vsilyaev
 * PR 42984: Wait for I/O completion shall be unconditional
 *
 * 15   5/21/08 2:37p erickson
 * PR41960: NEXUS_Playback_Start wasn't resetting its state to allow for
 *  the next Start.
 *
 * 14   5/15/08 4:38p erickson
 * PR42119: default trickmode_params correctly
 *
 * 13   5/7/08 4:20p erickson
 * PR42329: unlock playback inside NEXUS_Playback_Stop to avoid deadlock
 *  with playpump callbacks
 *
 * 12   5/5/08 5:42p vsilyaev
 * PR 42355: Fixed transition from trickmode to frame advance/reverse
 *
 * 11   5/5/08 4:46p vsilyaev
 * PR 42355: Fixed rewind for AVI files
 *
 * 10   4/25/08 1:24p erickson
 * PR41951: NEXUS_Playback_ClosePidChannel should return void
 *
 * 9   4/17/08 4:26p vsilyaev
 * PR 41845: Hold playpump callbacks if not playing data
 *
 * 8   4/10/08 5:08p erickson
 * PR40832: fix eWaitingRecord state
 *
 * 7   4/10/08 9:16a erickson
 * PR41589: check rc
 *
 * 6   4/9/08 1:09p jgarrett
 * PR 41557: Fixing GetDefaultPidChannelSettings
 *
 * 5   4/7/08 12:04p erickson
 * PR41431: only fail stcChannel change during trick mode
 *
 * 4   3/17/08 4:09p vsilyaev
 * PR 40554: In broadcom trikcmodes use decode_rate from the media player
 *
 * 3   3/8/08 7:45a erickson
 * PR40103: convert to NEXUS_TaskCallback
 *
 * 2   3/7/08 5:34p erickson
 * PR40307: don't return error on success
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/26   1/8/08 2:23p vsilyaev
 * PR 35824: Fixed resync on transition between modes
 *
 * Nexus_Devel/25   1/7/08 5:09p erickson
 * PR35824: add new continuous record support
 *
 * Nexus_Devel/24   12/28/07 5:06p erickson
 * PR37574: return state in status
 *
 * Nexus_Devel/23   12/21/07 5:55p vsilyaev
 * PR 38073: Fixed fa -> fb -> play transitions
 *
 * Nexus_Devel/22   12/20/07 5:24p vsilyaev
 * PR 38073: Improved handling of trickmodes
 *
 * Nexus_Devel/21   12/20/07 3:41p vsilyaev
 * PR 38073: Improving use of bmedia_player
 *
 * Nexus_Devel/20   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 *
 * Nexus_Devel/PR38073/3   12/19/07 5:01p vsilyaev
 * PR 38073: Moved time tracking code for host paced mode into the
 * bmedia_player
 *
 * Nexus_Devel/PR38073/2   12/18/07 6:45p vsilyaev
 * PR 38073: Added handling of no-index streams
 *
 * Nexus_Devel/PR38073/1   12/18/07 4:57p vsilyaev
 * PR 38073: Updated playback module to work exclusively with media player
 *
 * Nexus_Devel/19   12/6/07 5:24p vsilyaev
 * PR 37574: Added handling of segmented feed
 *
 * Nexus_Devel/18   12/5/07 3:09p vsilyaev
 * PR 37934: Implemented BLST_S_DICT
 *
 * Nexus_Devel/17   12/3/07 4:43p erickson
 * PR36404: restore start_paused code. fix status fifoSize.
 *
 * Nexus_Devel/16   11/16/07 12:38p vsilyaev
 * PR 35824: Added STC trickmodes
 *
 * Nexus_Devel/15   11/13/07 3:31p vsilyaev
 * PR 37015: At start Seek index to 0 offset
 *
 * Nexus_Devel/14   11/13/07 12:21p erickson
 * PR36788: fixed frameadvance. disabled start_paused check.
 *
 * Nexus_Devel/13   11/9/07 6:02p vsilyaev
 * PR 36788: Added timeshifting
 *
 * Nexus_Devel/12   11/1/07 9:41a erickson
 * PR36633: base enum changes
 *
 * Nexus_Devel/11   10/30/07 5:05p vsilyaev
 * PR 36404: Added audio tracking for slow motion modes
 *
 * Nexus_Devel/10   10/19/07 11:21a vsilyaev
 * PR 35824: Fixed data corruption on trick modes
 *
 * Nexus_Devel/9   10/17/07 5:38p vsilyaev
 * PR 35824: Added trickmode test
 *
 * Nexus_Devel/8   10/17/07 1:05p vsilyaev
 * PR 35824: Added playpump_trick
 *
 * Nexus_Devel/7   10/16/07 4:59p vsilyaev
 * PR 35824: Rearranged code
 *
 * Nexus_Devel/6   10/16/07 12:59p vsilyaev
 * PR 35824: Splitting playback into managable piecies
 *
 * Nexus_Devel/5   10/15/07 5:12p vsilyaev
 * PR 35824: Added module initialization
 *
 * Nexus_Devel/4   10/15/07 2:55p vsilyaev
 * PR 35824: Added synchronization thunk layer
 *
 * Nexus_Devel/2   10/11/07 6:25p vsilyaev
 * PR 35824: Added more playback code
 *
 * Nexus_Devel/1   10/10/07 4:54p vsilyaev
 * PR 35824: Playback module
 *
 * $copied_brcm_Log: /BSEAV/api/src/pvr/bsettop_playback.c $
 * $copied_brcm_Revision: 172 $
 * $copied_brcm_Date: 10/1/07 11:13a $
 **************************************************************************/
#include "nexus_playback_module.h"
#include "nexus_playback_impl.h"
#include "nexus_core_utils.h"

BDBG_MODULE(nexus_playback);

/* We have a separate dbg macro for data flow. When you want to see it, uncomment
the following */
#define BDBG_MSG_FLOW(X) /* BDBG_MSG(X) */



BDBG_OBJECT_ID(NEXUS_Playback);

/*
 * the following function is used to control playback dataflow
 * and handle various transition between trickmodes
 */
static NEXUS_Error bplay_p_frameadvance(NEXUS_PlaybackHandle p, bool forward, bool *restart);
static void b_play_playpump_read_callback_locked(void *context);
static NEXUS_Error b_play_playpump_read_callback_guard(void *context);

void
NEXUS_Playback_GetDefaultSettings( NEXUS_PlaybackSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->playpump = NULL;
    NEXUS_Playpump_GetDefaultSettings(&pSettings->playpumpSettings);
    pSettings->endOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    pSettings->stcChannel = NULL;
    pSettings->beginningOfStreamAction = NEXUS_PlaybackLoopMode_eLoop;
    NEXUS_CallbackDesc_Init(&pSettings->endOfStreamCallback);
    NEXUS_CallbackDesc_Init(&pSettings->beginningOfStreamCallback);
    NEXUS_CallbackDesc_Init(&pSettings->errorCallback);
    NEXUS_CallbackDesc_Init(&pSettings->parsingErrorCallback);
    pSettings->startPaused = false;
    pSettings->timeshifting = false;
    pSettings->stcTrick = false;
    pSettings->accurateSeek = false;
    pSettings->enableStreamProcessing = false;
    pSettings->ioTimeout = 5000; /* 5 sec */
    pSettings->playErrorHandling = NEXUS_PlaybackErrorHandlingMode_eEndOfStream;
    pSettings->seekErrorHandling = NEXUS_PlaybackErrorHandlingMode_eIgnore;
    pSettings->trickErrorHandling = NEXUS_PlaybackErrorHandlingMode_eIgnore;
    /* milliseconds to gap between end of playback and record which will not cause decode stuttering */
    pSettings->timeshiftingSettings.endOfStreamGap = 3000;
    /* milliseconds to gap between beginning of playback and truncating record which will not cause excess beginningOfStream events */
    pSettings->timeshiftingSettings.beginningOfStreamGap = 5000;

    return;
}

NEXUS_PlaybackHandle
NEXUS_Playback_Create( void )
{
    NEXUS_PlaybackHandle playback;
    NEXUS_Error rc;

    playback = BKNI_Malloc(sizeof(*playback));
    if (!playback) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto err_alloc; }

    BDBG_OBJECT_INIT(playback,NEXUS_Playback);
    BKNI_Memset(&playback->state, 0, sizeof(playback->state));

    playback->state.state = eStopped;
    BLST_S_INIT(&playback->pid_list);
    playback->file = NULL;
    playback->media_player = NULL;
    playback->thread_terminate = false;
    playback->playpump_thread_event = NULL;
    playback->playpump_thread = NULL;
    playback->playpump_event_callback = NULL;
    playback->playpump_event = NULL;
    playback->index_file_mode = NEXUS_PlaybackIndexFileIo_eCallback;
    playback->actualTransportType = NEXUS_TransportType_eTs;
    NEXUS_CallbackHandler_Init(playback->dataCallbackHandler, b_play_playpump_read_callback_locked, playback);
    NEXUS_CallbackHandler_SetGuard(playback->dataCallbackHandler, b_play_playpump_read_callback_guard);
    NEXUS_CallbackHandler_Init(playback->videoDecoderFirstPts, NEXUS_Playback_P_VideoDecoderFirstPts, playback);
    NEXUS_CallbackHandler_Init(playback->videoDecoderFirstPtsPassed, NEXUS_Playback_P_VideoDecoderFirstPtsPassed, playback);

    NEXUS_Thread_GetDefaultSettings(&playback->playpump_thread_settings);
    NEXUS_Playback_GetDefaultTrickModeSettings(&playback->state.trickmode_params);
    playback->errorCallback = NEXUS_TaskCallback_Create(playback, NULL);
    if(!playback->errorCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_error_callback;}
    playback->endOfStreamCallback = NEXUS_TaskCallback_Create(playback, NULL);
    if(!playback->endOfStreamCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_end_of_stream_callback;}
    playback->beginningOfStreamCallback = NEXUS_TaskCallback_Create(playback, NULL);
    if(!playback->beginningOfStreamCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_beginning_of_stream_callback;}
    playback->parsingErrorCallback = NEXUS_TaskCallback_Create(playback, NULL);
    if(!playback->parsingErrorCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_parsing_error_callback;}

    NEXUS_Playback_GetDefaultSettings(&playback->params);

    rc = BKNI_CreateEvent(&playback->ack_event);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_ack_event;}

    playback->playpump_event = NULL;
    playback->playpump_event_callback = NULL;

    b_play_media_time_test();
#if B_PLAYBACK_CAPTURE
    playback->capture.has_data = false;
    playback->capture.no = 0;
    playback->capture.fout = NULL;
#endif

    return playback;

err_ack_event:
    NEXUS_TaskCallback_Destroy(playback->parsingErrorCallback);
err_parsing_error_callback:
    NEXUS_TaskCallback_Destroy(playback->beginningOfStreamCallback);
err_beginning_of_stream_callback:
    NEXUS_TaskCallback_Destroy(playback->endOfStreamCallback);
err_end_of_stream_callback:
    NEXUS_TaskCallback_Destroy(playback->errorCallback);
err_error_callback:
    BKNI_Free(playback);
err_alloc:
    return NULL;
}


void
NEXUS_Playback_ClosePidChannel(NEXUS_PlaybackHandle playback, NEXUS_PidChannelHandle pidChannel)
{
    NEXUS_Error rc;
    NEXUS_Playback_P_PidChannel *play_pid;

    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);

    BLST_S_DICT_FIND(&playback->pid_list, play_pid, pidChannel, pidChn, link);
    if(play_pid==NULL) {
        BDBG_WRN(("NEXUS_Playback_ClosePidChannel: %#lx can't found pid:%#lx", (unsigned long)playback, (unsigned long)pidChannel));
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }

    if (play_pid->cfg.pidSettings.pidType == NEXUS_PidType_eVideo) {
        NEXUS_VideoDecoderPlaybackSettings playbackSettings;
        NEXUS_P_Playback_VideoDecoder_GetPlaybackSettings(play_pid, &playbackSettings);
        playbackSettings.firstPts.callback = NULL;
        playbackSettings.firstPtsPassed.callback = NULL;
        (void)NEXUS_P_Playback_VideoDecoder_SetPlaybackSettings(play_pid, &playbackSettings);
        NEXUS_CallbackHandler_Stop(playback->videoDecoderFirstPts);
        NEXUS_CallbackHandler_Stop(playback->videoDecoderFirstPtsPassed);
    }

        NEXUS_CallbackHandler_Stop(playback->videoDecoderFirstPts);
        NEXUS_CallbackHandler_Stop(playback->videoDecoderFirstPtsPassed);
        BLST_S_DICT_REMOVE(&playback->pid_list, play_pid, pidChannel, NEXUS_Playback_P_PidChannel, pidChn, link);
        BDBG_ASSERT(play_pid);
        BDBG_ASSERT(playback->params.playpump);
        rc = NEXUS_Playpump_ClosePidChannel(playback->params.playpump, play_pid->pidChn);
        if(rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc);}
        BKNI_Free(play_pid);
    return;
}

void
NEXUS_Playback_CloseAllPidChannels(NEXUS_PlaybackHandle playback)
{
    NEXUS_Playback_P_PidChannel *pid;

    /* clear all pid channels */
    while(NULL!=(pid = BLST_S_FIRST(&playback->pid_list))) {
        /* use NEXUS_Playback_ClosePidChannel so that other cleanup will happen */
        NEXUS_Playback_ClosePidChannel(playback, pid->pidChn);
    }
    return;
}

void
NEXUS_Playback_GetDefaultPidChannelSettings(NEXUS_PlaybackPidChannelSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_Playpump_GetDefaultOpenPidChannelSettings(&pSettings->pidSettings);
    BDBG_ASSERT(pSettings->pidSettings.pidType == NEXUS_PidType_eUnknown);
    return;
}

NEXUS_PidChannelHandle
NEXUS_Playback_OpenPidChannel(NEXUS_PlaybackHandle playback, unsigned pidNo, const NEXUS_PlaybackPidChannelSettings *pSettings)
{
    NEXUS_PlaybackPidChannelSettings settings;
    NEXUS_Playback_P_PidChannel *play_pid;
    NEXUS_Error rc;
    unsigned playpump_pidNo;

    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    if(pSettings==NULL) {
        NEXUS_Playback_GetDefaultPidChannelSettings(&settings);
        pSettings = &settings;
    }

    if(playback->params.playpump==NULL) {
        BDBG_WRN(("NEXUS_Playback_OpenPidChannel: %#lx pid could only be open if playpump is attached", (unsigned long)playback));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_playpump;
    }
    play_pid = BKNI_Malloc(sizeof(*play_pid));
    if(play_pid==NULL) { rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    playpump_pidNo = pidNo;

    switch(playback->params.playpumpSettings.transportType) {
    default:
        break;
#if 0
    case NEXUS_TransportType_eMp4:
    case NEXUS_TransportType_eMkv:
        break;
#endif
    case NEXUS_TransportType_eEs:
        if(playback->params.enableStreamProcessing) {
            playpump_pidNo = 0xC0;
        }
        break;
    }

    BDBG_MSG(("NEXUS_Playback_OpenPidChannel: %#lx pid %#lx mapped %u->%#x", (unsigned long)playback, (unsigned long)play_pid, (unsigned)pidNo, (unsigned)playpump_pidNo));
    play_pid->pidChn = NEXUS_Playpump_OpenPidChannel(playback->params.playpump, playpump_pidNo, &pSettings->pidSettings);
    if(!play_pid->pidChn) { rc = BERR_TRACE(BERR_NOT_SUPPORTED);goto err_open_pid;}
    play_pid->pid = pidNo;
    play_pid->cfg = *pSettings;

    BLST_S_DICT_ADD(&playback->pid_list, play_pid, NEXUS_Playback_P_PidChannel, pidChn, link, err_duplicate);

    return play_pid->pidChn;

err_duplicate:
    rc = BERR_TRACE(BERR_INVALID_PARAMETER);
    NEXUS_Playpump_ClosePidChannel(playback->params.playpump, play_pid->pidChn);
err_open_pid:
    BKNI_Free(play_pid);
err_alloc:
err_playpump:
    return NULL;

}

NEXUS_Error
NEXUS_Playback_GetPidChannelSettings(NEXUS_PlaybackHandle playback, NEXUS_PidChannelHandle pidChannel, NEXUS_PlaybackPidChannelSettings *pSettings)
{
    const NEXUS_Playback_P_PidChannel *pid;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    BDBG_ASSERT(pSettings);

    BLST_S_DICT_FIND(&playback->pid_list, pid, pidChannel, pidChn, link);
    if(!pid) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_invalid_pid;
    }
    *pSettings = pid->cfg;
    return BERR_SUCCESS;
err_invalid_pid:
    return rc;
}

NEXUS_Error
NEXUS_Playback_SetPidChannelSettings( NEXUS_PlaybackHandle playback, NEXUS_PidChannelHandle pidChannel, const NEXUS_PlaybackPidChannelSettings *pSettings)
{
    NEXUS_Playback_P_PidChannel *pid;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    BDBG_ASSERT(pSettings);

    BLST_S_DICT_FIND(&playback->pid_list, pid, pidChannel, pidChn, link);
    if(!pid) { rc = BERR_TRACE(NEXUS_INVALID_PARAMETER); goto err_invalid_pid; }

    if(BKNI_Memcmp(&pid->cfg.pidSettings, &pSettings->pidSettings, sizeof(pSettings->pidSettings))!=0) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_settings; } /* we can't change playpump pid configuration */
    pid->cfg.pidTypeSettings = pSettings->pidTypeSettings;

    if (playback->state.state != eStopped) {
        /* reapply current decoder settings */
        rc = b_play_trick_set_pid(playback, pid, NULL);
        if (rc) {rc = BERR_TRACE(rc); goto err_settings;}
    }

    return BERR_SUCCESS;

err_settings:
err_invalid_pid:
    return rc;
}

static void
NEXUS_Playback_P_KillPlaypumpThread(NEXUS_PlaybackHandle playback)
{
    playback->thread_terminate = true;
    BDBG_ASSERT(playback->playpump_thread_event);
    BKNI_SetEvent(playback->playpump_thread_event);
    NEXUS_UnlockModule();
    BDBG_MSG(("killing playpump_thread ..."));
    NEXUS_Thread_Destroy(playback->playpump_thread);
    BDBG_MSG(("killing playpump_thread ... done"));
    NEXUS_LockModule();
    playback->playpump_thread = NULL;
    return;
}


void
NEXUS_Playback_Destroy( NEXUS_PlaybackHandle playback)
{
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);

    if(playback->state.state != eStopped) {
        BDBG_WRN(("NEXUS_Playback_Destroy: destroying playback without stopping"));
        NEXUS_Playback_Stop(playback);
    }
    NEXUS_Playback_CloseAllPidChannels(playback);

    if(playback->playpump_thread) {
        NEXUS_Playback_P_KillPlaypumpThread(playback);
    }
    if(playback->playpump_thread_event) {
        BKNI_DestroyEvent(playback->playpump_thread_event);
    }
    if(playback->playpump_event) {
        BDBG_ASSERT(playback->playpump_event_callback);
        NEXUS_UnregisterEvent(playback->playpump_event_callback);
        BKNI_DestroyEvent(playback->playpump_event);
    }
    BKNI_DestroyEvent(playback->ack_event);
    NEXUS_CallbackHandler_Shutdown(playback->dataCallbackHandler);
    NEXUS_CallbackHandler_Shutdown(playback->videoDecoderFirstPts);
    NEXUS_CallbackHandler_Shutdown(playback->videoDecoderFirstPtsPassed);
    NEXUS_TaskCallback_Destroy(playback->parsingErrorCallback);
    NEXUS_TaskCallback_Destroy(playback->errorCallback);
    NEXUS_TaskCallback_Destroy(playback->endOfStreamCallback);
    NEXUS_TaskCallback_Destroy(playback->beginningOfStreamCallback);

    BDBG_OBJECT_DESTROY(playback, NEXUS_Playback);
    BKNI_Free(playback);
    return;
}


static int
NEXUS_P_Playback_GetPriority(void *cntx)
{
    NEXUS_PlaybackHandle playback=cntx;
    NEXUS_PlaypumpStatus status;
    NEXUS_Error rc;
    int factor;

    /* no b_lock here because the state check is atomic and bplaypump_get_status
    has its own synchronization. */
    if (playback->state.mode == NEXUS_PlaybackState_ePlaying) {
        BDBG_ASSERT(playback->params.playpump);

        rc = NEXUS_Playpump_GetStatus(playback->params.playpump, &status);
        if(rc==NEXUS_SUCCESS) {
            factor  = (status.fifoSize - status.fifoDepth)/(status.fifoSize/256); /* priority is 0 .. 256 */
            /* make read priority grow linearly with space in the fifo*/
            BDBG_MSG_FLOW(("[%#x] level %d%% priority %d", playback, status.fifoDepth*100/status.fifoSize, factor));
     } else {
            factor = 0;
        }
    } else {
        factor = 32;
    }

    return factor;
}


/**
Relocate BNAV_Player to the current decode location.
Call these before changing pvr state in order to get the current pts.
**/
void
b_play_update_location(NEXUS_PlaybackHandle p)
{
    NEXUS_Error rc;
    uint32_t pts;

    rc = b_play_getpts(p, &pts);
    if(rc==NEXUS_SUCCESS) {
        p->state.validPts = true;
        bmedia_player_update_position(p->media_player, pts);
    } else {
        p->state.validPts = false;
        BDBG_MSG(("Can't reset location because no pts."));
    }
    return;
}

const NEXUS_Playback_P_PidChannel *
b_play_get_video_decoder(NEXUS_PlaybackHandle playback)
{
    const NEXUS_Playback_P_PidChannel *pid;
    for(pid = BLST_S_FIRST(&playback->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        if(pid->cfg.pidSettings.pidType == NEXUS_PidType_eVideo) {
            if (pid->cfg.pidTypeSettings.video.decoder || pid->cfg.pidTypeSettings.video.simpleDecoder) return pid;
        }
    }
    return NULL;
}

static bool
b_play_has_audio_decoder(NEXUS_PlaybackHandle playback)
{
    const NEXUS_Playback_P_PidChannel *pid;
    for(pid = BLST_S_FIRST(&playback->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        if(pid->cfg.pidSettings.pidType == NEXUS_PidType_eAudio) {
            if (pid->cfg.pidTypeSettings.audio.primary || pid->cfg.pidTypeSettings.audio.secondary || pid->cfg.pidTypeSettings.audio.simpleDecoder) {
                return true;
            }
        }
    }
    return false;
}

bool
b_play_decoders_in_tsm_mode(NEXUS_PlaybackHandle playback)
{
    NEXUS_Error rc;
    const NEXUS_Playback_P_PidChannel *pid;
    for(pid = BLST_S_FIRST(&playback->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
        if(pid->cfg.pidSettings.pidType == NEXUS_PidType_eVideo) {
            NEXUS_VideoDecoderStatus status;
            rc = NEXUS_P_Playback_VideoDecoder_GetStatus(pid, &status);
            if (rc) return false;
            if (!status.tsm) return false;
        }
        else if(pid->cfg.pidSettings.pidType == NEXUS_PidType_eAudio) {
            NEXUS_AudioDecoderStatus status, status2;
            rc = NEXUS_P_Playback_AudioDecoder_GetStatus(pid, &status, &status2);
            if (rc) return false;
            if (status.started && !status.tsm) return false;
            if (status2.started && !status2.tsm) return false;
        }
    }
    return true;
}

NEXUS_Error
b_play_getpts(NEXUS_PlaybackHandle p, uint32_t *pts)
{
    NEXUS_Error rc;
    NEXUS_Error result=NEXUS_NOT_SUPPORTED;
    const NEXUS_Playback_P_PidChannel *pid;

    BDBG_ASSERT(pts);
    *pts = 0;

    /* check video before checking audio */
    pid = b_play_get_video_decoder(p);
    if (pid) {
        NEXUS_VideoDecoderStatus status;
        rc = NEXUS_P_Playback_VideoDecoder_GetStatus(pid, &status);
        if(rc==NEXUS_SUCCESS) {
            if(status.ptsType == NEXUS_PtsType_eCoded || status.ptsType == NEXUS_PtsType_eInterpolatedFromValidPTS || p->actualTransportType==NEXUS_TransportType_eEs) {
                *pts = status.pts;
                result = NEXUS_SUCCESS;
            }
        }
    } else {
        bool foundDecoder = false;

        for(pid = BLST_S_FIRST(&p->pid_list); pid ; pid = BLST_S_NEXT(pid, link)) {
            if(pid->cfg.pidSettings.pidType == NEXUS_PidType_eAudio) {
                NEXUS_AudioDecoderStatus audioStatus, audioStatus2;
                rc = NEXUS_P_Playback_AudioDecoder_GetStatus(pid, &audioStatus, &audioStatus2);
                    if(rc==BERR_SUCCESS) {
                    if(audioStatus.started && (audioStatus.ptsType == NEXUS_PtsType_eCoded || audioStatus.ptsType == NEXUS_PtsType_eInterpolatedFromValidPTS || p->actualTransportType==NEXUS_TransportType_eEs)) {
                            *pts = audioStatus.pts;
                        result = NEXUS_SUCCESS;
                        foundDecoder = true;
                        break;
                    }
                    if(audioStatus2.started && (audioStatus2.ptsType == NEXUS_PtsType_eCoded || audioStatus2.ptsType == NEXUS_PtsType_eInterpolatedFromValidPTS)) {
                        *pts = audioStatus2.pts;
                        result = NEXUS_SUCCESS;
                        foundDecoder = true;
                        break;
                    }
                    if ( audioStatus.started || audioStatus2.started ) {
                        /* Found a valid pid but no PTS available yet */
                        foundDecoder = true;
                    }
                }
            }
        }
        if(!foundDecoder && !p->state.noDecoderWarning) {
            BDBG_WRN(("Unable to look up position because no audio decoder was provided in any NEXUS_PlaybackPidChannelSettings."));
            p->state.noDecoderWarning = true; /* only print once per start */
        }
    }
    return result;
}



void
NEXUS_Playback_GetDefaultTrickModeSettings(NEXUS_PlaybackTrickModeSettings *trickmode)
{
    BKNI_Memset(trickmode, 0, sizeof(*trickmode));
    trickmode->mode = NEXUS_PlaybackHostTrickMode_eNone;
    trickmode->rate = NEXUS_NORMAL_PLAY_SPEED;
    trickmode->mode_modifier = 1;
    trickmode->maxDecoderRate = 1.2 * NEXUS_NORMAL_PLAY_SPEED;
    trickmode->rateControl = NEXUS_PlaybackRateControl_eDecoder;
    trickmode->skipControl = NEXUS_PlaybackSkipControl_eHost;
    trickmode->brcmTrickMode = true;
    return;
}

static int bplay_p_accurate_seek(NEXUS_PlaybackHandle p, bmedia_player_pos position)
{
    uint32_t pts = 0;
    const NEXUS_Playback_P_PidChannel *pid = b_play_get_video_decoder(p);
    NEXUS_VideoDecoderHandle decode;
    int rc;
    b_trick_settings settings;

    /* requires regular decoder. does not support simpleDecoder */
    if (!pid) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    decode = pid->cfg.pidTypeSettings.video.decoder;
    if (!decode) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(p->file==NULL || p->file->file.index==NULL) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED); /* playback should be started with an index */
    }

    /* must flush before NEXUS_VideoDecoder_SetStartPts is called to avoid a false detect
    with existing pictures in the queue */
    b_play_flush(p);

    switch (p->params.playpumpSettings.transportType) {
    case NEXUS_TransportType_eTs:
        if(p->state.index_type!=NEXUS_PlaybackMpeg2TsIndexType_eNav) {
            /* not supported for self-indexed MPEG-2 TS files */
            return BERR_TRACE(NEXUS_NOT_SUPPORTED);
        }
        break;
    case NEXUS_TransportType_eAsf:
    case NEXUS_TransportType_eAvi:
        break;
    default:
        /* not supported for other containers
        for MKV and MP4, get_next_frame is an asynchronous function. we will need to devise a different technique. */
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    b_play_trick_get(p, &settings);
    if (settings.state != b_trick_state_normal) {
        /* only applies to normal play mode */
        return 0;
    }

    if (!bmedia_player_lookup_pts(p->media_player, position, &pts)) {

        switch (p->params.playpumpSettings.transportType) {
        case NEXUS_TransportType_eAsf:
        case NEXUS_TransportType_eAvi:
            {
            bmedia_player_decoder_mode mode;
            bmedia_player_entry entry;
            /* jump back to the previous key frame */
            bmedia_player_set_direction(p->media_player, -33, -BMEDIA_TIME_SCALE_BASE, &mode);
            do {
                if (bmedia_player_next(p->media_player, &entry)) {
                    entry.timestamp = position;
                    break;
                }
            }
            while (entry.type == bmedia_player_entry_type_file && entry.start == 0);
            bmedia_player_set_direction(p->media_player, 0, BMEDIA_TIME_SCALE_BASE, &mode);
            bmedia_player_seek(p->media_player, entry.timestamp);
            BDBG_WRN(("accurate seek: from timestamp %d to %d (pts %#x)", entry.timestamp, position, pts));
            }
            break;
        default:
            /* bcmplayer already seeks back to previous I frame */
            BDBG_WRN(("accurate seek: to %d (pts %#x)", position, pts));
            break;
        }

        /* tell decoder to discard until this pts */
        rc = NEXUS_VideoDecoder_SetStartPts(decode, pts);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        BDBG_WRN(("Unable to do accurateSeek position=%d(%#x), decode=%p", position, pts, decode));
    }

    /* start state machine to allow audio to chase video's PTS. This keeps a full audio CDB from
    hanging video as it seeks for its start PTS. the state machine is:
    1) wait for video first PTS
    2) start audio pause (which launches the rap_monitor_timer)
    3) wait for video first PTS passed, which means we've found the start pts
    4) stop audio pause
    */
    p->state.inAccurateSeek = true;
    return 0;
}

/*
Resume normal playback. This should not be called immediately after bplay_start because it's
not needed.

We may come from the following states:
1) stc trick modes.
2) decoder pause from decoder trick or normal play.
3) decoder pause from host trick. requires reset_location.
4) decoder trick.
5) host trick. requires reset_location.

The way to distinguish 2 & 3 is to use the BNAV_Player state. See below.
*/
static NEXUS_Error
bplay_p_play_impl(NEXUS_PlaybackHandle p, bool *restart, bool flush, bmedia_player_pos position)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    b_trick_settings settings;
    bmedia_player_decoder_mode mode;
    bmedia_player_decoder_config config;

    *restart = false;

    if (flush) {
        b_play_flush(p);
    }

    /* XXX this should be avoided in favor of calling bmedia_player_set_direction, and then handling bmedia_player_decoder_mode */
    b_play_trick_get(p, &settings);
    settings.forward = true;
    settings.state = b_trick_state_normal;
    settings.decode_rate = NEXUS_NORMAL_PLAY_SPEED;
    settings.decode_mode = NEXUS_VideoDecoderDecodeMode_eAll;
    settings.fragmented = false;
    settings.reordering_mode = NEXUS_VideoDecoderReorderingMode_eNone;
    settings.stc_trick = p->params.stcTrick;
    settings.force_source_frame_rate = 0;
    settings.simulated_tsm = false;
    NEXUS_Playback_GetDefaultTrickModeSettings(&p->state.trickmode_params);
    rc = b_play_trick_set(p, &settings);
    if (rc != NEXUS_SUCCESS) { return BERR_TRACE(rc); }

    bmedia_player_get_decoder_config(p->media_player, &config);
    config.host_mode = bmedia_player_host_trick_mode_auto;
    config.mode_modifier = 1; /* unused */
    rc = bmedia_player_set_decoder_config(p->media_player, &config);
    if (rc) return BERR_TRACE(rc);

    if (p->state.decoder_flushed) {
        /* resync player */
        bmedia_player_set_direction(p->media_player, 0, BMEDIA_TIME_SCALE_BASE, &mode); /* normal decode */

        if (p->params.accurateSeek) {
            bplay_p_accurate_seek(p, position);
        }

        *restart = true;
    }

    p->state.direction = 1;
    p->state.mode = NEXUS_PlaybackState_ePlaying;
    p->state.frame_advance = NEXUS_Playback_P_FrameAdvance_Forward;
    return rc;
}

NEXUS_Error
bplay_p_play(NEXUS_PlaybackHandle p, bool *restart, bool flush)
{
    bmedia_player_pos position;

    BDBG_MSG(("resume play"));
    b_play_update_location(p);
    bmedia_player_tell(p->media_player, &position);
    return bplay_p_play_impl(p, restart, flush, position);
}

NEXUS_Error
bplay_p_play_from(NEXUS_PlaybackHandle p, bmedia_player_pos position)
{
    bool restart;
    bmedia_player_seek(p->media_player, position);
    return bplay_p_play_impl(p, &restart, true, position);
}


NEXUS_Error
bplay_p_pause(NEXUS_PlaybackHandle p)
{
    b_trick_settings settings;
    BDBG_MSG(("going to a pause, state.state %d, state.mode %d", p->state.state, p->state.mode));
    if (p->state.mode == NEXUS_PlaybackState_ePaused) {
        return NEXUS_SUCCESS;
    }
    p->state.mode = NEXUS_PlaybackState_ePaused;
    p->state.loopedDuringPause = false;

    /* during pause, we have a timer so we can notify the caller of internal changes trim continuous record trim */
    if (!p->state.timer) {
        p->state.timer = NEXUS_ScheduleTimer(B_FRAME_DISPLAY_TIME * 5, b_play_timer, p);   /* schedule another call into the same function after while */
    }

    b_play_trick_get(p, &settings);
    settings.decode_rate = 0;
    return b_play_trick_set(p, &settings);
}

/**
Callback from brecord
**/
void
NEXUS_Playback_RecordProgress_priv(NEXUS_PlaybackHandle p)
{
    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);
    NEXUS_ASSERT_MODULE();

    if (p->state.start_paused) {
        return;
    }

    if (p->state.state != eWaitingRecord) {
        BDBG_MSG_FLOW(("Got record event, but not waiting for record(%d), ignore", p->state.state));
        if (p->state.state == eWaitingPlayback) {
            goto restart; /* Kickstart playback */
        }
        return;
    }

    BDBG_MSG(("NEXUS_Playback_RecordProgress_priv %d", p->state.read_size));
    if (p->state.read_size==0) {
        bmedia_player_pos pos;
        bmedia_player_status status;

        bmedia_player_tell(p->media_player, &pos);
        bmedia_player_get_status(p->media_player, &status);
        if (pos > status.bounds.first) {
            /* we are waiting for an index, try to get new entry */
            b_play_next_frame(p);
        }
        else {
            /* if we've fallen off the beginning of a continuous record, we need to reposition */
            p->state.state = eWaitingPlayback;
        }
    }
    else {
       /* we are waiting for a data */
        p->state.state = eWaitingIo;
#if defined(HUMAX_PLATFORM_BASE)
        b_play_update_buffer_status(p);
#endif	   
        NEXUS_File_AsyncRead(p->file->file.data, p->state.buf, p->state.read_size, NEXUS_MODULE_SELF, b_play_frame_data, p);
    }

restart:
    b_play_check_buffer(p);
    return;
}

/**
Whatever state you're in, get into a paused state that can be
used for frame advance. This means paused-from-play.
**/
static NEXUS_Error
bplay_p_get_into_paused_play_state(NEXUS_PlaybackHandle p, bool *restart)
{
    NEXUS_Error rc;
    rc = bplay_p_play(p, restart, true);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
    rc = bplay_p_pause(p);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
    *restart = true; /* we've already flushed, but we need to restart playback after this */
    p->state.frame_advance = NEXUS_Playback_P_FrameAdvance_Forward;
    return rc;
}

/* set b_trick_settings based on bmedia_player_decoder_mode and (optionally) a NEXUS_PlaybackTrickModeSettings override */
static void
NEXUS_Playback_P_ConvertPlayerDecodeMode(NEXUS_PlaybackHandle p, const bmedia_player_decoder_mode *mode,  b_trick_settings *trick_settings, const NEXUS_PlaybackTrickModeSettings *params)
{
    unsigned abs_time_scale;
    BDBG_CASSERT(NEXUS_VideoDecoderDecodeMode_eAll==(NEXUS_VideoDecoderDecodeMode)bmedia_player_decoder_frames_all);
    BDBG_CASSERT(NEXUS_VideoDecoderDecodeMode_eIP==(NEXUS_VideoDecoderDecodeMode)bmedia_player_decoder_frames_IP);
    BDBG_CASSERT(NEXUS_VideoDecoderDecodeMode_eI==(NEXUS_VideoDecoderDecodeMode)bmedia_player_decoder_frames_I);
    trick_settings->decode_mode = (NEXUS_VideoDecoderDecodeMode)mode->display_frames;

    BDBG_CASSERT(NEXUS_VideoDecoderReorderingMode_eNone==(NEXUS_VideoDecoderReorderingMode)bmedia_player_reordering_mode_none);
    BDBG_CASSERT(NEXUS_VideoDecoderReorderingMode_eSequential==(NEXUS_VideoDecoderReorderingMode)bmedia_player_reordering_mode_sequential);
    BDBG_CASSERT(NEXUS_VideoDecoderReorderingMode_eInterleaved==(NEXUS_VideoDecoderReorderingMode)bmedia_player_reordering_mode_interleaved);
    BDBG_CASSERT(NEXUS_VideoDecoderReorderingMode_eChunkForward==(NEXUS_VideoDecoderReorderingMode)bmedia_player_reordering_mode_forward);
    BDBG_CASSERT(NEXUS_VideoDecoderReorderingMode_eChunkBackward==(NEXUS_VideoDecoderReorderingMode)bmedia_player_reordering_mode_backward);
    trick_settings->reordering_mode = (NEXUS_VideoDecoderReorderingMode)mode->reordering_mode;
    trick_settings->simulated_tsm = false;
    abs_time_scale = mode->time_scale >= 0 ? mode->time_scale : -mode->time_scale;
    if(mode->continuous) {
        trick_settings->decode_rate = abs_time_scale * (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE);
        trick_settings->state = b_trick_state_normal;
    } else {
        trick_settings->decode_rate = NEXUS_NORMAL_PLAY_SPEED;
        if(mode->brcm) {
             trick_settings->state = b_trick_state_brcm_trick_mode;
             trick_settings->decode_rate = (abs_time_scale * (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE));
        } else if(mode->dqt) {
             trick_settings->state = b_trick_state_display_queue_trick_mode;
        } else if(mode->tsm) {
            trick_settings->decode_rate = mode->time_scale * (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE);
            trick_settings->state = b_trick_state_host_paced;
        } else if(mode->simulated_tsm) {
            trick_settings->decode_rate = params?params->rate:NEXUS_NORMAL_PLAY_SPEED;
            trick_settings->state = b_trick_state_host_trick_mode;
            trick_settings->simulated_tsm = true;
        } else if(mode->fragmented) {
            trick_settings->decode_rate = abs_time_scale * (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE);
            trick_settings->state = b_trick_state_host_trick_mode;
            trick_settings->reordering_mode = trick_settings->forward ? NEXUS_VideoDecoderReorderingMode_eChunkForward : NEXUS_VideoDecoderReorderingMode_eChunkBackward;
        } else if(mode->reordering_mode!=bmedia_player_reordering_mode_none) {
            trick_settings->decode_rate = abs_time_scale * (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE);
            trick_settings->state = b_trick_state_host_trick_mode;
        } else {
            trick_settings->decode_rate = abs_time_scale * (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE);
            trick_settings->state = b_trick_state_host_trick_mode;
        }
    }
    /* apply custom modifications */
    if (params && params->mode!=NEXUS_PlaybackHostTrickMode_eNone) {
        /* override rate control */
        /* decode_rate is unsigned. decoder and STC rate can only be positive. in the future, SW STC could support negative rates. */
        trick_settings->decode_rate = params->rate>0?params->rate:-params->rate;
        trick_settings->stc_trick = (params->rateControl == NEXUS_PlaybackRateControl_eStream && p->params.stcChannel);
        switch(params->mode) {
        case NEXUS_PlaybackHostTrickMode_ePlayI:
            trick_settings->decode_mode = NEXUS_VideoDecoderDecodeMode_eI;
            break;
        case NEXUS_PlaybackHostTrickMode_ePlayIP:
            trick_settings->decode_mode = NEXUS_VideoDecoderDecodeMode_eIP;
            break;
        default:
            break;
        }
    } else {
        trick_settings->stc_trick = p->params.stcTrick;
    }
    return;
}

/**
The type of frame advance depends on the type of pause used. You must always be paused
before doing frame advance/reverse.

Frame advance (i.e. forward) is can be decoder-based or stc based.
Frame reverse is host-based.
**/
static NEXUS_Error
bplay_p_frameadvance(NEXUS_PlaybackHandle p, bool forward, bool *restart)
{
    NEXUS_Error rc;

    BDBG_MSG(("frame %s", forward?"advance":"reverse"));
    *restart = false;

    /* fifoMarkerCounter tells the wait_for_end routine to not check for a certain amount of time after a frame advance.
    If your system checks for static video PTS to detect EOF, this will give it time to move. */
    p->state.loopedDuringPause = false;
    if (p->state.fifoMarkerCounter == 0) {
        unsigned cdbDepth;

        bplay_get_decode_mark(p, &p->state.fifoMarker, &cdbDepth);
        p->state.fifoMarkerCounter = 5;
    }

    if(forward && p->state.frame_advance != NEXUS_Playback_P_FrameAdvance_Forward) {
        rc = bplay_p_get_into_paused_play_state(p, restart);
        if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
    } else if(!forward && p->state.frame_advance != NEXUS_Playback_P_FrameAdvance_Reverse) {
        bmedia_player_decoder_mode mode;
        b_trick_settings settings;

        b_play_trick_get(p, &settings);
        b_play_update_location(p);
        bmedia_player_set_direction(p->media_player, -33 /* 33 ms */, -BMEDIA_TIME_SCALE_BASE, &mode);
        NEXUS_Playback_P_ConvertPlayerDecodeMode(p, &mode,  &settings, NULL);
        if(mode.discontinuity) {
            b_play_flush(p);
        }
        p->state.frame_advance = NEXUS_Playback_P_FrameAdvance_Reverse;
        *restart = true;
        settings.decode_rate = 0;
        settings.forward = false;
        b_play_trick_set(p, &settings);
    } else {
        BDBG_MSG(("continuing frame %s", forward?"advance":"reverse"));
    }

    p->state.direction = forward?1:-1;

    rc = b_play_trick_frameadvance(p);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);}
    p->state.mode = NEXUS_PlaybackState_ePaused;
    return rc;
}

static NEXUS_Error NEXUS_Playback_P_CheckWaitingIo(NEXUS_PlaybackHandle playback, bool stopping)
{
    unsigned i = 0;
    unsigned ioTimeout = playback->params.ioTimeout; /* exposed in the NEXUS_PlaybackSettings if we support recovery from io error by killing the File i/o thread. */

/* period of 1/10th of a second */
#define IO_TIMEOUT_PERIOD 100
    if (playback->state.state == eWaitingIo) { /* WaitingIo it's non cancelable state, so we should wait for I/O to complete */
        playback->state.state = stopping ? eStopping : eCancelIo;
        BDBG_MSG(("NEXUS_Playback_P_CheckWaitingIo %#lx waiting for I/O to complete", playback));
        do {
            NEXUS_Error rc;
            NEXUS_UnlockModule();
            rc = BKNI_WaitForEvent(playback->ack_event, IO_TIMEOUT_PERIOD);
            NEXUS_LockModule();
            if (rc == BERR_TIMEOUT) {
                BDBG_WRN(("NEXUS_Playback_P_CheckWaitingIo %#lx timed out, keep waiting", (unsigned long)playback));
            }
            else if (rc!=0) {
                return BERR_TRACE(rc);
            }
            if (++i >= ioTimeout / IO_TIMEOUT_PERIOD) {
                BDBG_ERR(("NEXUS_Playback_P_CheckWaitingIo %#lx timed out. Canceling File thread.", (unsigned long)playback));
                NEXUS_FilePlay_Cancel(playback->file);
                i = 0;
                /* we keep looping. File will spawn a new thread and will invoke the callback with an error. Playback should go to eAborted. */
            }
        } while (!(playback->state.state == eIoCanceled || playback->state.state == eStopped || playback->state.state == eAborted));
    }
    if (playback->state.state == eAborted) {
        return -1;
    }
    return 0;
}


/*
* This function is used to synchronize control with
* data pump. it return b_ok if synchronization suceeded,
* false otherwise.
*/
static NEXUS_Error
bplay_sync_playback(NEXUS_PlaybackHandle p)
{
    BERR_Code rc;

    if (p->state.start_paused) {
        /* if playback wasn't started yet, just do nothing. we should be in WaitingPlayback state. */
        BDBG_ASSERT(p->state.state == eWaitingPlayback);
        return NEXUS_SUCCESS;
    }
    if (p->state.state == eStopped) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    if (p->state.state == eAborted) {
        /* something went wrong. we need the app to stop. */
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    rc = NEXUS_Playback_P_CheckWaitingIo(p, false);
    if (rc) return BERR_TRACE(rc);

    p->state.reset = false;

    /* still locked */
    return NEXUS_SUCCESS;
}

/* this function is used to jumpstart data pump if it was cancelled because of bplay_sync_playback */
static void
bplay_restart_playback(NEXUS_PlaybackHandle p, bool restart_data_flow)
{
    if (p->state.start_paused) {
        /* if playback wasn't started yet, just do nothing. we should be in WaitingPlayback state. */
        BDBG_ASSERT(p->state.state == eWaitingPlayback);
        return;
    }

    /* if restart_data_flow is false, then either the API has failed or we're in a paused state and
    don't want to restart. in all other cases, it's true. */
    if (restart_data_flow) {
        /* 3 states override this */
        restart_data_flow = (
            p->state.state != eIoCanceled &&
            p->state.state != eWaitingPlayback &&
            p->state.state != eWaitingRecord);
    }

    if (p->state.reset) {
        BDBG_MSG(("I/O reset was requested"));
        if(p->state.media.entry.atom) {
            batom_release(p->state.media.entry.atom);
            p->state.media.entry.atom = NULL;
        }
        b_play_flush(p);
        bplay_p_clear_buffer(p);
        p->state.state = eTransition;
        /* start data flowing */
        b_play_next_frame(p);
    }
    /* PR 21643 - The eWaitingRecord exception was added because if you pause immediately after starting a timeshift record,
    you could be in waiting record state. we need to restart. */
    else if (restart_data_flow)
    {
        BDBG_MSG(("Restart: start data flowing"));
        b_play_next_frame(p);
    }
    else if (p->state.state == eWaitingRecord)
    {
        BDBG_MSG(("Restart: start data flowing in eWaitingRecord"));
        if (p->state.read_size==0) {
            /* we are waiting for an index, try to get new entry */
            b_play_next_frame(p);
        }
        else {
           /* we are waiting for a data */
            p->state.state = eWaitingIo;
            NEXUS_File_AsyncRead(p->file->file.data, p->state.buf, p->state.read_size, NEXUS_MODULE_SELF, b_play_frame_data, p);
        }
    }
    else if (p->state.state == eIoCanceled)  {
        BDBG_MSG(("Restart: I/O was canceled, reviving it"));
        p->state.state = eWaitingIo;
        switch(p->state.io_size) {
        case B_MEDIA_ATOM_MAGIC:
            b_play_media_send_meta(p);
            break;
        case B_MEDIA_NEST_MAGIC:
            b_play_next_frame(p);
            break;
       default:
            b_play_frame_data(p, p->state.io_size);
            break;
        }
    }
    else if (p->state.state == eWaitingPlayback ||
             p->state.state == eTimer)
    {
        /* This section of code needs to handle all normal states that require no change. */
        BDBG_MSG(("Restart: no state change state=%d,mode=%d", p->state.state, p->state.mode));
    }
    else {
        /* If this occurs, something is broken in the state machine and must be fixed immediately. This
        is complex code and small changes can lead to disastrous lockups and reentrancy. */
        BDBG_ERR(("Restart: unknown state %d", p->state.state));
    }

    p->state.decoder_flushed = false;
    b_play_check_buffer(p);
    return;
}

/* this function is used to abort playback if trick/play/seek functions have failed, it's should be called instead of bplay_restart_playback */
void
NEXUS_Playback_P_AbortPlayback(NEXUS_PlaybackHandle p)
{
    BDBG_WRN(("NEXUS_Playback_P_AbortPlayback: %#x aborting", (unsigned)p));
    if (p->state.start_paused) {
        /* if playback wasn't started yet, just do nothing. we should be in WaitingPlayback state. */
        BDBG_ASSERT(p->state.state == eWaitingPlayback);
        return;
    }

    /* save playback position */
    b_play_update_location(p);
    bmedia_player_tell(p->media_player, &p->state.abortPosition);

    b_play_flush(p);
    if(p->state.media.entry.atom) {
        batom_release(p->state.media.entry.atom);
        p->state.media.entry.atom = NULL;
    }
    p->state.state = eAborted;
    p->state.decoder_flushed = false;
    return;
}


/* rest of functions is a public API to control playback */
NEXUS_Error
NEXUS_Playback_Play(NEXUS_PlaybackHandle p)
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);

    if (p->state.start_paused) {
        BDBG_MSG(("NEXUS_Playback_Play: %#lx starting playback from paused state", (unsigned long)p));
        p->state.start_paused = false;
        p->state.data_source(p);
        return NEXUS_SUCCESS;
    }

    if (p->state.mode == NEXUS_PlaybackState_ePlaying) {
        BDBG_WRN(("Already in the playback mode"));
        return NEXUS_SUCCESS;
    }

    rc = bplay_sync_playback(p);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc); goto done; }

    rc = bplay_p_play(p, &p->state.reset, false);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);} /* fall through so bplay_restart_playback is called */

    if(rc==NEXUS_SUCCESS || p->params.seekErrorHandling==NEXUS_PlaybackErrorHandlingMode_eIgnore) {
        bplay_restart_playback(p, rc==NEXUS_SUCCESS);
    } else {
        NEXUS_Playback_P_AbortPlayback(p);
        rc = BERR_TRACE(rc);
    }
done:
    return rc;
}


NEXUS_Error
NEXUS_Playback_Pause(NEXUS_PlaybackHandle p)
{
    NEXUS_Error rc=NEXUS_SUCCESS;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);

    if (p->state.start_paused) {
        BDBG_MSG(("NEXUS_Playback_Play: %#lx starting playback from paused state", (unsigned long)p));
        p->state.start_paused = false;
        p->state.data_source(p);
    } else if (p->state.mode == NEXUS_PlaybackState_ePaused) {
        BDBG_WRN(("Already in pause mode"));
        goto done;
    }

    rc = bplay_p_pause(p);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);} /* fall through */

done:
    return rc;
}

NEXUS_Error
NEXUS_Playback_FrameAdvance(NEXUS_PlaybackHandle p, bool forward)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);


    if (p->state.mode != NEXUS_PlaybackState_ePaused) {
        BDBG_WRN(("Not in paused state"));
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto done;
    }

    rc = bplay_sync_playback(p);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc); goto done; }

    rc = bplay_p_frameadvance(p, forward, &p->state.reset);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);} /* fall through so bplay_restart_playback is called */

    bplay_restart_playback(p, rc==BERR_SUCCESS && !p->state.drain_mode);

done:
    return rc;
}


static NEXUS_Error
b_play_set_media_player_config(NEXUS_PlaybackHandle p, const NEXUS_PlaybackTrickModeSettings *params)
{
    NEXUS_Error rc;
    bmedia_player_decoder_config config;

    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_eNone == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_auto);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_eNormal ==  (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_normal);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_ePlayI == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_I);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_ePlaySkipB == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_skipB);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_ePlayIP == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_IP);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_ePlaySkipP == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_skipP);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_ePlayBrcm == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_brcm);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_ePlayGop == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_gop);
    BDBG_CASSERT( NEXUS_PlaybackHostTrickMode_eTimeSkip == (NEXUS_PlaybackHostTrickMode)bmedia_player_host_trick_mode_time_skip);

    bmedia_player_get_decoder_config(p->media_player, &config);
    b_play_update_media_player_config(p, &config);

    if (params->mode != NEXUS_PlaybackHostTrickMode_eNone && params->skipControl == NEXUS_PlaybackSkipControl_eDecoder) {
        switch(params->mode) {
        case NEXUS_PlaybackHostTrickMode_eNormal:
        case NEXUS_PlaybackHostTrickMode_ePlayI:
        case NEXUS_PlaybackHostTrickMode_ePlayIP:
            config.host_mode = bmedia_player_host_trick_mode_auto;
            break;
        default:
            /* decoder doesn't support other skip modes */
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    else {
    config.host_mode = params->mode;
    }

    config.mode_modifier = params->mode_modifier;
    config.max_decoder_rate = params->maxDecoderRate/(NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE);
    if (params->mode != NEXUS_PlaybackHostTrickMode_eNone) {
        config.stc = (params->rateControl == NEXUS_PlaybackRateControl_eStream && p->params.stcChannel);
    }
    else {
        /* p->params.stcTrick is only used for automatic trick modes */
    config.stc = p->params.stcTrick;
    }
    config.brcm = params->brcmTrickMode;
    rc = bmedia_player_set_decoder_config(p->media_player, &config);
    if (rc) return BERR_TRACE(rc);

    return rc;
}


NEXUS_Error
NEXUS_Playback_TrickMode(NEXUS_PlaybackHandle p, const NEXUS_PlaybackTrickModeSettings *params)
{
    NEXUS_Error rc;
    int new_direction;
    b_trick_settings trick_settings;
#if defined(HUMAX_PLATFORM_BASE) && defined (CONFIG_PVR_PLAYMODE_TIMESKIP)
    bmedia_time_scale t_time_scale;
    int denom_c;
#endif
    bmedia_player_decoder_mode mode;
    NEXUS_PlaybackTrickModeSettings modified_params;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);
    if (params->mode >= NEXUS_PlaybackHostTrickMode_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (params->skipControl == NEXUS_PlaybackSkipControl_eDecoder || params->rateControl == NEXUS_PlaybackRateControl_eDecoder) {
    if (!b_play_get_video_decoder(p) && !b_play_has_audio_decoder(p)) {
        BDBG_ERR(("NEXUS_Playback_TrickMode failed because no video or audio decoder was provided in any NEXUS_PlaybackPidChannelSettings."));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    }

    /* validate params based on automatic or custom mode */
    if (params->mode == NEXUS_PlaybackHostTrickMode_eNone) {
        /* in automatic mode, validate all custom trick params are left at default values */
        if (params->rateControl != NEXUS_PlaybackRateControl_eDecoder ||
            params->skipControl != NEXUS_PlaybackSkipControl_eHost ||
            params->mode_modifier != 1)
        {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        new_direction = (params->rate >= 0)?1:-1;
    }
    else {
        /* verify custom mode params */
        if (params->skipControl == NEXUS_PlaybackSkipControl_eDecoder &&
            params->mode_modifier != 1) {
            /* decoder can't do anything with the mode modifier, so ensure its default */
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        if (params->mode_modifier >= 0 && params->rate < 0) {
            /* do not make this conversion. fix the app. */
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        if (params->mode_modifier < 0 && params->rate >= 0) {
            /* grandfather this conversion */
            BDBG_WRN(("changing trick rate from %d to %d because mode_modifier is negative", params->rate, -1*params->rate));
            modified_params = *params;
            modified_params.rate = -modified_params.rate;
            params = &modified_params;
        }
        /* we may support negative eStream rates in the future using SW STC. */
        new_direction = (params->rate >= 0)?1:-1;
    }

    rc = bplay_sync_playback(p);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc); goto done; }

    BDBG_CASSERT( (256 * NEXUS_NORMAL_PLAY_SPEED)/BMEDIA_TIME_SCALE_BASE == 256 * (NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE));

    p->state.start_paused = false;
    
    b_play_update_location(p);

    rc = b_play_set_media_player_config(p, params);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}

    /* set host player rate */
    {
        bmedia_player_step direction;
        bmedia_time_scale time_scale;
        int player_rate;

        if (params->skipControl == NEXUS_PlaybackSkipControl_eDecoder) {
            /* if skipControl is eDecoder, then we aren't skipping in the host. */
            player_rate = NEXUS_NORMAL_PLAY_SPEED;
        }
        else {
    player_rate = params->rate;
        }

        direction = (33*player_rate)/NEXUS_NORMAL_PLAY_SPEED;
        if (player_rate && !direction) { /* prevent rounding down to zero */
            direction = player_rate>0?1:-1;
        }
        time_scale = player_rate/(NEXUS_NORMAL_PLAY_SPEED/BMEDIA_TIME_SCALE_BASE);
        if (player_rate && !time_scale) { /* prevent rounding down to zero */
            time_scale = player_rate>0?1:-1;
        }

#if defined(HUMAX_PLATFORM_BASE) && defined (CONFIG_PVR_PLAYMODE_TIMESKIP)
        t_time_scale = time_scale;
#endif
        rc = bmedia_player_set_direction(p->media_player, direction, time_scale, &mode);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error;}
    }

    /* set decoder/stc rate */
    b_play_trick_get(p, &trick_settings);

    NEXUS_Playback_P_ConvertPlayerDecodeMode(p, &mode, &trick_settings, params);
    trick_settings.forward = (new_direction==1);
    trick_settings.avoid_flush = (params->avoidFlush || !p->file->file.index );
    trick_settings.force_source_frame_rate = mode.force_source_frame_rate;
    trick_settings.maxFrameRepeat = params->maxFrameRepeat;

#if defined(HUMAX_PLATFORM_BASE) && defined (CONFIG_PVR_PLAYMODE_TIMESKIP)
    if(t_time_scale < 0 && t_time_scale > -100) /* slow-motion case */
    {
        if(mode.brcm == false) /*  HD(AVC) case */
        {
            denom_c = ((-750)/t_time_scale);
            if(denom_c <= 0)
                denom_c = 1;
            trick_settings.decode_rate = 250/denom_c;
        }
        else /* SD(MPEG2) case */
        {
            trick_settings.decode_rate = trick_settings.decode_rate * 100 / 150;
        }
        if(trick_settings.decode_rate == 0)
        {
            trick_settings.decode_rate = 1;
        }
    }
#endif
    rc = b_play_trick_set(p, &trick_settings);
    if (rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc); } /* fall through. we need to maintain internal Playback state, even on a bad trick mode setting. */

    if(mode.discontinuity) {
        b_play_flush(p);
    }

    if (p->state.decoder_flushed) {
        p->state.reset = true;
    }

    if (rc==NEXUS_SUCCESS) {
        p->state.trickmode_params = *params;
        p->state.direction = new_direction;
        p->state.mode = NEXUS_PlaybackState_eTrickMode;
    }

error:
    if(rc==NEXUS_SUCCESS || p->params.seekErrorHandling==NEXUS_PlaybackErrorHandlingMode_eIgnore) {
        bplay_restart_playback(p, rc==NEXUS_SUCCESS);
    } else {
        NEXUS_Playback_P_AbortPlayback(p);
        rc = BERR_TRACE(rc);
    }

done:
    if(params->rate == -NEXUS_NORMAL_PLAY_SPEED) {
        p->state.frame_advance = NEXUS_Playback_P_FrameAdvance_Reverse;
    } else if(params->rate >=0 && params->rate <= NEXUS_NORMAL_PLAY_SPEED) {
        p->state.frame_advance = NEXUS_Playback_P_FrameAdvance_Forward;
    } else {
        p->state.frame_advance = NEXUS_Playback_P_FrameAdvance_Invalid;
    }
    return rc;
}

NEXUS_Error
NEXUS_Playback_Seek(NEXUS_PlaybackHandle p, NEXUS_PlaybackPosition position)
{
    NEXUS_Error rc;
    int result;

    BDBG_OBJECT_ASSERT(p, NEXUS_Playback);
    rc = bplay_sync_playback(p);
    if(rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc); goto done;}

    if (!b_play_get_video_decoder(p) && !b_play_has_audio_decoder(p)) {
        /* no-decoder playback is valid, so this is only a WRN */
        BDBG_WRN(("No video or audio decoder was provided with NEXUS_PlaybackPidChannelSettings; therefore NEXUS_Playback_Seek will not be precise."));
    }

    result = bmedia_player_seek(p->media_player, position);

    if (result!=-1 && p->params.accurateSeek && !p->state.start_paused) {
        bplay_p_accurate_seek(p, position);
    }

    if (result!=-1) {
        p->state.reset = true;
    }
    else {
        /* don't use BERR_TRACE. failed seek is normal. */
        rc = NEXUS_UNKNOWN;
    }
    if(rc!=NEXUS_SUCCESS) {
        if(p->params.seekErrorHandling==NEXUS_PlaybackErrorHandlingMode_eAbort) {
            NEXUS_Playback_P_AbortPlayback(p);
            rc = BERR_TRACE(rc);
            goto done;
        }
    }

    bplay_restart_playback(p, false);
done:
    return rc;
}

NEXUS_Error
NEXUS_Playback_GetStatus(NEXUS_PlaybackHandle p, NEXUS_PlaybackStatus *status)
{
    NEXUS_PlaypumpStatus pumpstatus;
    NEXUS_Error rc=NEXUS_SUCCESS;
    bmedia_player_status player_status;
    bmedia_player_pos position;
    bool need_checkbuffer = false;

    BKNI_Memset(status, 0, sizeof(*status));
    NEXUS_Playback_GetDefaultTrickModeSettings(&status->trickModeSettings);
    switch(p->state.state) {
    case eStopped:
        status->state = NEXUS_PlaybackState_eStopped;
        break;
    case eAborted:
        status->state = NEXUS_PlaybackState_eAborted;
        break;
    default:
        status->state = p->state.mode;
        status->trickModeSettings = p->state.trickmode_params;
        if(!p->state.start_paused) {
            need_checkbuffer = true;
        } else {
            status->state = NEXUS_PlaybackState_ePaused;
        }
        if(status->state == NEXUS_PlaybackState_ePaused) {
            status->trickModeSettings.rate = 0;
        }
        break;
    }

    if (p->state.state != eStopped) {
        BDBG_ASSERT(p->params.playpump);
        rc = NEXUS_Playpump_GetStatus(p->params.playpump, &pumpstatus);
        if(rc==NEXUS_SUCCESS) {
            status->fifoDepth = pumpstatus.fifoDepth;
            status->fifoSize = pumpstatus.fifoSize;
            status->bytesPlayed = pumpstatus.bytesPlayed;
        }
        bmedia_player_get_status(p->media_player, &player_status);
        status->first = player_status.bounds.first;
        status->last = player_status.bounds.last;
        status->indexErrors = p->state.index_error_cnt + player_status.index_error_cnt;
        status->dataErrors = p->state.data_error_cnt + player_status.data_error_cnt;
        status->readPosition = player_status.position;

        if (p->state.state != eAborted) {
            if(!p->state.start_paused) {
                b_play_update_location(p);
            }
            bmedia_player_tell(p->media_player, &position);
            status->position = position;
        }
        else {
            status->position = p->state.abortPosition;
        }
    }
    if(need_checkbuffer) {
        b_play_check_buffer(p);
    }

    return rc;
}

void
NEXUS_Playback_GetDefaultStartSettings(NEXUS_PlaybackStartSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->mode = NEXUS_PlaybackMode_eIndexed;
    pSettings->bitrate = 10 * 1000 * 1000;
    pSettings->mpeg2TsIndexType = NEXUS_PlaybackMpeg2TsIndexType_eAutoDetect;
    pSettings->indexFileIo.mode = NEXUS_PlaybackIndexFileIo_eCallback;
    NEXUS_Thread_GetDefaultSettings(&pSettings->indexFileIo.threadSettings);
    return;
}

static void
NEXUS_Playback_P_PlaypumpThread(void *context)
{
    NEXUS_PlaybackHandle playback = context;
    for(;;) {
        BERR_Code rc;
        BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
        BDBG_ASSERT(playback->playpump_thread_event);
        rc=BKNI_WaitForEvent(playback->playpump_thread_event, 100);
        if(!(rc==BERR_SUCCESS || rc==BERR_TIMEOUT)) {
            rc=BERR_TRACE(rc);
            break;
        }
        NEXUS_LockModule();
        BDBG_ASSERT(playback->playpump_thread_event);
        if(playback->thread_terminate) {
            NEXUS_UnlockModule();
            break;
        }
        if(rc==BERR_SUCCESS && playback->state.state != eStopped) {
            b_play_read_callback(playback);
        }
        NEXUS_UnlockModule();
    }
    BDBG_MSG(("NEXUS_Playback_P_PlaypumpThread: %#x terminated", (unsigned)playback));
    return;
}

NEXUS_Error
NEXUS_Playback_Start(NEXUS_PlaybackHandle playback, NEXUS_FilePlayHandle file, const NEXUS_PlaybackStartSettings *pSettings)
{
    NEXUS_PlaypumpStatus playpump_status;
    NEXUS_Error rc=NEXUS_SUCCESS;
    NEXUS_PlaypumpSettings pumpCfg;
    NEXUS_PlaybackStartSettings defaultStartSettings;

    if (!file) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);

    if(!pSettings) {
        NEXUS_Playback_GetDefaultStartSettings(&defaultStartSettings);
        pSettings = &defaultStartSettings;
    }
    b_play_capture_open(playback);

    playback->state.media.entry.atom = NULL;

    if (playback->state.state != eStopped) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error_state;}
    if (playback->params.playpump == NULL)  { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error_state;}
    if (file->file.data == NULL) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto error_state;}

    BDBG_MSG(("NEXUS_Playback_Start: %#lx", playback));
    BKNI_Memset(&playback->state, 0, sizeof(playback->state)); /* wipe-out all temporary state */
    playback->state.validPts = false;
    playback->state.drain_mode = false;
    playback->state.frame_advance = NEXUS_Playback_P_FrameAdvance_Forward;

    switch(pSettings->indexFileIo.mode) {
    case NEXUS_PlaybackIndexFileIo_eCallback:
        /* do nothing */
        break;
    case NEXUS_PlaybackIndexFileIo_eModule:
        if(playback->playpump_event == NULL) {
            rc = BKNI_CreateEvent(&playback->playpump_event);
            if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto error_index_mode;}
            playback->playpump_event_callback = NEXUS_RegisterEvent(playback->playpump_event, b_play_read_callback, playback);
            if(playback->playpump_event_callback==NULL) {
                rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
                BKNI_DestroyEvent(playback->playpump_event);
                playback->playpump_event=NULL;
                goto error_index_mode;
            }
        }
        break;
    case NEXUS_PlaybackIndexFileIo_eThread:
        if(playback->playpump_thread_event == NULL) {
            rc = BKNI_CreateEvent(&playback->playpump_thread_event);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error_index_mode;}
        }
        if(playback->playpump_thread) {
            if(BKNI_Memcmp(&playback->playpump_thread_settings, &pSettings->indexFileIo.threadSettings, sizeof(pSettings->indexFileIo.threadSettings))==0) {
                break; /* thread configuration is the same, keep on using old thread */
            }
            NEXUS_Playback_P_KillPlaypumpThread(playback);
        }
        {
            char thread_name[32];
            BKNI_Snprintf(thread_name, sizeof(thread_name), "nx_playback%u", (unsigned)playback);
            playback->playpump_thread = NEXUS_Thread_Create(thread_name, NEXUS_Playback_P_PlaypumpThread, playback, &pSettings->indexFileIo.threadSettings);
            if(!playback->playpump_thread) { rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY); goto error_index_mode; }
        }
        break;
    default:
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto error_index_mode;
    }
    playback->index_file_mode = pSettings->indexFileIo.mode;

    b_play_trick_init(playback);
    /* trick must know about stcTrick now in case of pause */
    playback->trick.settings.stc_trick = playback->params.stcTrick;
    /* reset any currently attached decoders */
    rc = b_play_trick_set(playback, NULL);
    if (rc) {rc = BERR_TRACE(rc); goto error_state;}

    playback->state.state = eStopped;
    playback->file = file;
    NEXUS_Time_Get(&playback->state.fifoLast); /* get init value */

    rc = NEXUS_Playpump_GetStatus(playback->params.playpump, &playpump_status);
    if (rc) {rc = BERR_TRACE(rc); goto error_state;}

    playback->buf_limit = (uint8_t*)playpump_status.bufferBase + playpump_status.fifoSize - B_IO_BLOCK_LIMIT;
    BDBG_MSG(("playback buffer %#lx(%d)",
        (unsigned long)playpump_status.bufferBase, playpump_status.fifoSize));

    bio_read_attach_priority(playback->file->file.data, NEXUS_P_Playback_GetPriority, playback);

    playback->state.mode = NEXUS_PlaybackState_ePlaying;
    playback->state.encrypted = false ; /* mpeg->encryption.type != bencryption_type_none; */

    playback->state.state = eTransition;
    playback->media_player = NULL;
    /* seek to the beginning of the file */
    playback->file->file.data->seek(playback->file->file.data, 0, SEEK_SET);

    NEXUS_Playpump_GetSettings(playback->params.playpump, &pumpCfg);
    pumpCfg.mode = NEXUS_PlaypumpMode_eFifo;
    rc = NEXUS_Playpump_SetSettings(playback->params.playpump, &pumpCfg);
    if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto error_playpump_mode; }

    rc = b_play_start_media(playback, file, &playpump_status, pSettings);
    if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto error_player;}

    NEXUS_StartCallbacks(playback->params.playpump);
    /* assume that playback and decode will start in normal play */
    NEXUS_Playback_GetDefaultTrickModeSettings(&playback->state.trickmode_params);

    playback->state.state = eWaitingPlayback;
    playback->state.start_paused = playback->params.startPaused;
    rc = NEXUS_Playpump_Start(playback->params.playpump);
    if (rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto error_playpump; }

    return BERR_SUCCESS;

error_playpump:
    b_play_stop_media(playback);
error_playpump_mode:
error_player:
error_state:
error_index_mode:
    playback->state.state = eStopped;
    /* On any Start failure, we must end in a clean eStopped state that can be restarted. */
    return rc;
}

void
NEXUS_Playback_GetSettings(NEXUS_PlaybackHandle playback, NEXUS_PlaybackSettings *settings)
{
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    BDBG_ASSERT(settings);
    *settings = playback->params;
    return;
}

static NEXUS_Error
b_play_playpump_read_callback_guard(void *context)
{
    NEXUS_PlaybackHandle playback = (NEXUS_PlaybackHandle)context;


    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    /* no need to acquire lock here, since this mode is only changing when starting playback */
    if(playback->state.state == eStopped) {
        return NEXUS_CALLBACK_DEFLECTED;
    }
    switch(playback->index_file_mode) {
    case NEXUS_PlaybackIndexFileIo_eCallback:
        return NEXUS_SUCCESS;


        break;
    case NEXUS_PlaybackIndexFileIo_eModule:
        BKNI_SetEvent(playback->playpump_event); /* set event and do work inside module's context */
        break;
    case NEXUS_PlaybackIndexFileIo_eThread:
        BKNI_SetEvent(playback->playpump_thread_event); /* set event and do work inside thread */
        break;
    default:
        BDBG_ASSERT(0);
        break;
    }
    return NEXUS_CALLBACK_DEFLECTED;
}

static void
b_play_playpump_read_callback_locked(void *context)
{
    b_play_read_callback(context);
}

NEXUS_Error
NEXUS_Playback_SetSettings(NEXUS_PlaybackHandle playback, const NEXUS_PlaybackSettings *settings)
{
    NEXUS_Error  rc;

    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    BDBG_ASSERT(settings);

    if(settings->endOfStreamAction >= NEXUS_PlaybackLoopMode_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(settings->beginningOfStreamAction >= NEXUS_PlaybackLoopMode_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(settings->playErrorHandling>=NEXUS_PlaybackErrorHandlingMode_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(settings->seekErrorHandling>=NEXUS_PlaybackErrorHandlingMode_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(settings->trickErrorHandling>=NEXUS_PlaybackErrorHandlingMode_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(settings->playErrorHandling!=NEXUS_PlaybackErrorHandlingMode_eEndOfStream
        && settings->playErrorHandling!=NEXUS_PlaybackErrorHandlingMode_eAbort) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    if (settings->stcTrick && !settings->stcChannel) {
        BDBG_ERR(("NEXUS_PlaybackSettings.stcTrick requires NEXUS_PlaybackSettings.stcChannel"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (settings->stcTrick != playback->params.stcTrick && playback->state.state != eStopped) {
        BDBG_ERR(("You must set NEXUS_PlaybackSettings.stcTrick before calling NEXUS_Playback_Start"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (playback->state.state != eStopped && (playback->state.mode == NEXUS_PlaybackState_eTrickMode || playback->state.trickmode_params.rate != NEXUS_NORMAL_PLAY_SPEED)) {
        if(settings->stcChannel != playback->params.stcChannel) {
            BDBG_ERR(("Cannot change Playback's stcChannel during a trick mode."));
            rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
            goto err_settings;
        }
    }

    /* settings that can't change if we are started or there's a pid opened */
    if(playback->state.state != eStopped || BLST_S_FIRST(&playback->pid_list)) {
        if (settings->playpump != playback->params.playpump) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_settings;}
        /* in playpumpSettings, the only setting that's currently sensitive to pidchannels being opened is transportType. this could change. */
        if (settings->playpumpSettings.transportType != playback->params.playpumpSettings.transportType) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_settings;}
    }
    
        if(settings->playpump) {
           NEXUS_PlaypumpSettings pumpCfg = settings->playpumpSettings;
            /* replace dataCallback, and don't leak it outside */
           NEXUS_CallbackHandler_PrepareCallback(playback->dataCallbackHandler, pumpCfg.dataCallback);
           pumpCfg.originalTransportType = NEXUS_TransportType_eUnknown;
           switch(pumpCfg.transportType) {
#if 0
           case NEXUS_TransportType_eMp4:
           case NEXUS_TransportType_eMkv:
               pumpCfg.originalTransportType = pumpCfg.transportType;
               pumpCfg.transportType = NEXUS_TransportType_eMpeg2Pes;
               break;
#endif
           case NEXUS_TransportType_eEs:
                if(settings->enableStreamProcessing) {
                    pumpCfg.originalTransportType = pumpCfg.transportType;
                    pumpCfg.transportType = NEXUS_TransportType_eMpeg2Pes;
                }
               break;
           default:
               break;
           }

       playback->actualTransportType = pumpCfg.transportType;
           rc = NEXUS_Playpump_SetSettings(settings->playpump, &pumpCfg);
           if(rc!=NEXUS_SUCCESS) { rc = BERR_TRACE(rc); goto err_playpump; }
        }

    /* cannot call b_play_trick_set here. it is init'd in Start */

    NEXUS_TaskCallback_Set(playback->errorCallback, &settings->errorCallback);
    NEXUS_TaskCallback_Set(playback->endOfStreamCallback, &settings->endOfStreamCallback);
    NEXUS_TaskCallback_Set(playback->beginningOfStreamCallback, &settings->beginningOfStreamCallback);
    NEXUS_TaskCallback_Set(playback->parsingErrorCallback, &settings->parsingErrorCallback);

    if(playback->state.state != eStopped && (!settings->timeshifting && playback->params.timeshifting)) {
        playback->params.timeshifting = false;  /* clear timeshifting flag and simulate notification from record */
        NEXUS_Playback_RecordProgress_priv(playback);
    }

    playback->params = *settings;
    return NEXUS_SUCCESS;
err_playpump:
err_settings:
    return rc;
}

void
NEXUS_Playback_Stop(NEXUS_PlaybackHandle playback)
{
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    BDBG_MSG(("NEXUS_Playback_Stop: %#lx", playback));
    if(playback->state.state == eStopped) {
        BDBG_WRN(("NEXUS_Playback_Stop: %#lx playback already stoped", (unsigned long)playback));
        goto done;
    }

    /* if aborted, we can try to stop. */
    if(playback->state.state != eAborted) {
        rc = NEXUS_Playback_P_CheckWaitingIo(playback, true);
        if (rc) {
            BDBG_ERR(("Playback unable to stop. System is now unstable."));
            rc = BERR_TRACE(rc);
        }
    }

    /* stop timer before unlocking module so that no additional calls to Playpump are made after NEXUS_Playpump_Stop */
    if( playback->state.timer ) {
        BDBG_MSG(("Timer still active, cancelling %#x", playback->state.timer ));
        NEXUS_CancelTimer(playback->state.timer);
        playback->state.timer = NULL;
    }

    BDBG_ASSERT(playback->params.playpump);
    NEXUS_Playpump_Stop(playback->params.playpump);


    b_play_stop_media(playback);
    b_play_trick_shutdown(playback);
    b_play_capture_close(playback);
    playback->state.inAccurateSeek = false;
    playback->state.state = eStopped;

    NEXUS_StopCallbacks(playback->params.playpump);
    NEXUS_CallbackHandler_Stop(playback->dataCallbackHandler);
    NEXUS_CallbackHandler_Stop(playback->videoDecoderFirstPts);
    NEXUS_CallbackHandler_Stop(playback->videoDecoderFirstPtsPassed);    


done:
    return ;
}

void NEXUS_Playback_IsTransportTypeSupported( NEXUS_PlaybackHandle playback, NEXUS_TransportType transportType, bool *pIsSupported )
{
    BDBG_OBJECT_ASSERT(playback, NEXUS_Playback);
    if (playback->params.playpump) {
        switch (transportType) {
        case NEXUS_TransportType_eMp4:
        case NEXUS_TransportType_eMkv:
            *pIsSupported = true;
            break;
        default:
            NEXUS_Playpump_IsTransportTypeSupported(playback->params.playpump, transportType, pIsSupported);
            break;
        }
    }
    else {
        *pIsSupported = false;
    }
}
