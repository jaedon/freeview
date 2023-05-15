/***************************************************************************
*     (c)2003-2012 Broadcom Corporation
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
* $brcm_Workfile: b_playback_ip_trickmode.c $
* $brcm_Revision: 73 $
* $brcm_Date: 10/30/12 11:10a $
*
* Description: File contains RTSP & HTTP TrickMode support
*
* Revision History:
*
* $brcm_Log: /nexus/lib/playback_ip/src/b_playback_ip_trickmode.c $
* 
* 73   10/30/12 11:10a ssood
* SW7231-858: more changes to support & handle multiple PTS discontinuity
*  events
* 
* 72   10/16/12 5:25p ssood
* SW7231-858: update seek position only if caller seeked to a valid
*  position + no longer returning max duration -10sec in the demo mode
* 
* 71   9/19/12 9:31a ssood
* SW7425-3673: return new time position when seek over slower connections
*  is still in progress
* 
* 70   9/7/12 5:52p ssood
* SW7420-2358: fixed bug where seek to 0 offset would seek to current
*  position
* 
* 69   9/6/12 4:17p ssood
* SW7425-3673: added support for seek/pause/play of HLS sessions
* 
* 68   8/28/12 2:35p ssood
* SW7231-974: extend playback_ip current position logic to handle PTS
*  discontinuities during normal playback
* 
* 67   8/7/12 11:01a ssood
* SW7425-3694: fixed a regression in pause support when using server side
*  trickmodes
* 
* 66   6/28/12 4:59p ssood
* SW7425-3201: support for streaming trickmode content using playSpeed w/
*  non-Broadcom clients: program decoder to i-frame only mode
* 
* 65   6/20/12 4:36p ssood
* SW7231-858: suppressing more debug messages under ipVersboseLog
* 
* 64   6/8/12 7:11p ssood
* SW7425-3201: Add support for new DLNA PlaySpeed trickmode options in
*  playback ip library
* 
* 63   6/5/12 6:46p ssood
* SW7231-766: add support for downloading encrypted HLS keys + support
*  for HLS spec version 3
* 
* 62   5/23/12 3:13p ssood
* SW7425-2894: suppressing extra debug log from ip client modules
* 
* 61   5/18/12 4:31p ssood
* SW7425-3042: check for null playback handle
* 
* 60   1/11/12 2:50p ssood
* SW7425-2061: wrap simple decoders call for NEXUS_HAS_SIMPLE_DECODER
* 
* 59   12/30/11 12:00a ssood
* SW7231-516: dont provide position corresponding to EOF in demo mode
* 
* 58   12/28/11 1:12p ssood
* SW7425-2061: add simple decoder support to ip playback
* 
* 57   12/27/11 10:07p ssood
* SW7425-2061: add simple decoder support to ip playback
* 
* 56   12/1/11 6:41p mward
* SW7408-109: Compiler warning for unused parameter.
* 
* 55   11/23/11 2:10p ssood
* SW7408-109: RVU Trickplay support: added FFWD & FRWD
* 
* 54   10/22/11 10:46a ssood
* SW7231-405: fixed non-HLS compilation error
* 
* 53   10/21/11 1:54p ssood
* SW7231-405: pause support for HLS protocol
* 
* 52   9/26/11 10:22a ssood
* SW7346-341: Add PVR Decryption support for file streaming
* 
* 51   8/30/11 5:05p ssood
* SW7425-1077: set current time to 0 if we fail to obtain the current
*  position (happens when trickmode operations r done before decoder gets
*  to decode & have a pts)
* 
* 50   8/26/11 3:03p ssood
* SW7425-1077: fixed a pause race condition
* 
* 49   8/9/11 3:41p mward
* SW7125-1067:  Initialize the ipTrickModeSettings.  Assign it only if
*  used.
* 
* 48   6/28/11 11:11a ssood
* SW7231-227: decoder field reverse flag not being reset upon FR -> Play
*  transition
* 
* 47   6/6/11 1:28a ssood
* SW7231-166: added support for Slow Fwd, STC Based trick play at 2X, and
*  eventCallback support for end of stream playback
* 
* 46   6/3/11 5:29p ssood
* SW7231-166: set host trickmode flag for video decoder only in trickmode
*  states
* 
* 45   5/26/11 11:31p ssood
* SW7231-166: dont create Nexus File Handle for cases where Nexus
*  playpump will get used
* 
* 44   5/26/11 1:41p ssood
* SW7231-166: pause using connection stalling for live channels
* 
* 43   5/24/11 2:38p ssood
* SW7231-166: more changes to support DLNA trickmodes
* 
* 42   5/23/11 3:44p ssood
* SW7231-166: Extend Playback IP trickmodes to be compatible with latest
*  DLNA Guidelines
* 
* 41   12/23/10 7:41p ssood
* SW7420-1257: delay returning back to io thread when playback lock is
*  held by another thread
* 
* 40   11/22/10 9:21a ssood
* SW7420-1217: fixed compilation errors for non-HLS enabled builds
* 
* 39   11/19/10 6:59p ssood
* SW7420-1217: disable trickplay features for HLS protocol
* 
* 38   10/29/10 2:46p ssood
* SW7420-1217: conditionally compile HLS related changes to the existing
*  files
* 
* 37   10/29/10 9:05a ssood
* SW7420-1217: basic support to play both variant & non-variant bounded
*  playlists containing clear content
* 
* 36   10/27/10 8:36p ssood
* SW7125-353: coverity fix
* 
* 35   10/8/10 6:07p ssood
* SW7420-1088: add support for time based seeks
* 
* 34   10/6/10 6:27p ssood
* SW7420-1014: sometimes video will restart when changing FF rate + pause
*  macroblocking fix
* 
* 33   9/30/10 10:59a ssood
* SWGERRARD-547: only pause & resume before & after seek if currently in
*  the playing state
* 
* 32   9/24/10 5:25p ssood
* SWGERRARD-547: set flag to re-open the socket for index file descriptor
*  when seek is called
* 
* 31   9/17/10 8:21a ssood
* SWGERRARD-547: support to expose multiple multiple programs and tracks
*  within a multi-program stream
* 
* 30   9/7/10 1:20p ssood
* SW7420-876: fix regression to update timeline for live channels
* 
* 29   8/27/10 3:59p ssood
* SW7420-502: added partial support to play RVU sessions in live mode
* 
* 28   8/19/10 10:08a ssood
* SW7420-883: deadlock issue when seek internally calls play (which also
*  tries to get the same lock)
* 
* 27   8/12/10 11:31a ssood
* SW7420-502: disable seeks on TS playback when server side trickmodes
*  are supported
* 
* 26   7/29/10 2:39p ssood
* SW7420-716: reset IP playback state to original state if trickmode
*  operation fails
* 
* 25   6/16/10 5:05p ssood
* SW7420-716: initial RVU trickmode related check-ins
* 
* 24   4/2/10 5:56p ssood
* SW7468-198: Coverity fixes + RTSP related changes (requested by
*  Philips)
* 
* 23   1/23/10 10:17a ssood
* SW7420-454: further changes to condense the ip playback log
* 
* 22   12/29/09 6:39p ssood
* SW7420-454: obtain duration using PTS difference for TS streams
* 
* 21   12/17/09 11:42p ssood
* SW7420-454: fixed most macroblocking issues during the server side
*  trick mode transitions
* 
* 20   12/14/09 3:36p ssood
* SW7420-454: fix coverity reported issues
* 
* 19   12/8/09 8:56a ssood
* SW7420-454: use extra precision for current location calcuation while
*  transitioning to/from trickmodes
* 
* 18   11/18/09 7:16p ssood
* SW7420-340: set playback position to duration if playback reports
*  incorrect value
* 
* 17   10/15/09 11:34a ssood
* SW7405-1689: Enable trickmodes for VOB format
* 
* 16   8/24/09 3:23p ssood
* SW7405-1689: adapting server based trickmodes to Nexus playback changes
*  due to OTF support
* 
* 15   8/19/09 4:29p ssood
* PR50202: fix for making relative seek to work
* 
* 14   8/14/09 5:00p ssood
* PR50202: First check-in for clientside trickmode support for MPEG2 TS
*  format
* 
* 13   8/6/09 7:33p ssood
* PR48146: commented out a debug message
* 
* 12   8/6/09 10:40a ssood
* PR43746: fixing warnings
* 
* 11   8/6/09 10:18a ssood
* PR43746: added support for calculating more precise bitrates for TS &
*  VOB files (support index reads from middle & end of content)
* 
* 10   7/30/09 10:49a ssood
* PR43746: added support to play & seek MPEG2 Program Streams
* 
* 9   7/30/09 10:31a ssood
* PR43746: provide current playback position via the IP GetStatus
* 
* 8   7/29/09 6:14p ssood
* PR43746: extend IP trick modes to support Disconnect And Seek based
*  paused operations (DLNA requirement)
* 
* 7   7/21/09 4:18p ssood
* PR43746: Further work for Server based HTTP trick mode support
* 
* 6   7/16/09 12:59p ssood
* PR43746: Add Server based HTTP trick mode support
* 
* 5   8/14/08 3:16p lwhite
* PR42739: Corrected unused variable for LIVEMEDIA_SUPPORT=n
* 
* 4   8/5/08 7:59a ssood
* PR45400: make code compilable with -std=c89 for maximum compatibility
* 
* PR45400/1   8/5/08 12:44p dyzhang
* PR45400: kylin should compile with -std=c89 for maximum compatibility
* 
* 3   7/21/08 4:06p ssood
* PR42739: code changes to simplify ip app lib interface
* 
* 2   7/9/08 9:53p ssood
* PR42739: added support for high jitter + receiving variable size UDP
*  packets
* 
* 1   6/13/08 7:27p ssood
* PR42739: Adding new files for HTTP playback
* 
***************************************************************************/
#include "b_playback_ip_lib.h"
#include "b_playback_ip_priv.h"
#include "b_playback_ip_lm_helper.h"
#include "b_playback_ip_utils.h"
#include "nexus_audio_decoder_trick.h"

BDBG_MODULE(b_playback_ip_trickmode);
#if defined(LINUX) || defined(__vxworks)

extern B_PlaybackIpError http_do_server_trick_modes_socket_transition( B_PlaybackIpHandle playback_ip, double timeSeekRangeBegin, double timeSeekRangeEnd, int rate);
extern void print_av_pipeline_buffering_status(B_PlaybackIpHandle playback_ip);

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
extern B_PlaybackIpError B_PlaybackIp_PauseHls(B_PlaybackIpHandle playback_ip, B_PlaybackIpTrickModesSettings *ipTrickModeSettings);
extern B_PlaybackIpError B_PlaybackIp_PlayHls( B_PlaybackIpHandle playback_ip);
extern B_PlaybackIpError B_PlaybackIp_SeekHls( B_PlaybackIpHandle playback_ip, NEXUS_PlaybackPosition seekPosition);
#endif /* B_HAS_HLS_PROTOCOL_SUPPORT */

/* this function gets the pts value corresponding to the next video or audio frame being played */
B_PlaybackIpError 
http_get_current_pts(
    B_PlaybackIpHandle playback_ip, 
    unsigned int *currentPts
    )
{
    NEXUS_PlaypumpStatus ppStatus;

    if (playback_ip->nexusHandles.playpump) {
        if (NEXUS_Playpump_GetStatus(playback_ip->nexusHandles.playpump, &ppStatus) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: Failed to get Playpump staus", __FUNCTION__));
            return B_ERROR_UNKNOWN;
        }
        BDBG_MSG_FLOW(("PB buffer depth %d, size %d, fullness %d%%, played bytes %lld errors: range %u, sync %d, resync events %u stream errs %u", 
                    ppStatus.fifoDepth, ppStatus.fifoSize, (ppStatus.fifoDepth*100)/ppStatus.fifoSize, ppStatus.bytesPlayed,
                    ppStatus.pacingTsRangeError,
                    ppStatus.syncErrors,
                    ppStatus.resyncEvents,
                    ppStatus.streamErrors
                    ));
    }
    /* TODO: may need to base it using video PID instead */
    if (playback_ip->nexusHandles.videoDecoder) {
        NEXUS_VideoDecoderStatus status;
        if ((NEXUS_VideoDecoder_GetStatus(playback_ip->nexusHandles.videoDecoder, &status) != NEXUS_SUCCESS) || (status.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS)) {
            BDBG_ERR(("%s: %s", __FUNCTION__, (status.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS) ? "Got NEXUS_PtsType_eInterpolatedFromInvalidPTS, ignore it":"Failed to get current video PTS"));
            return B_ERROR_UNKNOWN;
        }
        *currentPts = status.pts;
        BDBG_MSG_FLOW(("q depth %d, decoded %d, displayed %d, decode errs %d, display errs %d, decode drops %d, display drops %d, display underflow %d, received %d, pts errs %d\n",
            status.queueDepth, status.numDecoded, status.numDisplayed, status.numDecodeErrors,
            status.numDisplayErrors, status.numDecodeDrops, status.numDisplayDrops, status.numDisplayUnderflows, status.numPicturesReceived, status.ptsErrorCount));
    }
#ifdef NEXUS_HAS_SIMPLE_DECODER
    else if (playback_ip->nexusHandles.simpleVideoDecoder) {
        NEXUS_VideoDecoderStatus status;
        if ((NEXUS_SimpleVideoDecoder_GetStatus(playback_ip->nexusHandles.simpleVideoDecoder, &status) != NEXUS_SUCCESS) || (status.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS)) {
            BDBG_ERR(("%s: %s", __FUNCTION__, (status.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS) ? "Got NEXUS_PtsType_eInterpolatedFromInvalidPTS, ignore it":"Failed to get current video PTS"));
            return B_ERROR_UNKNOWN;
        }
        *currentPts = status.pts;
        BDBG_MSG_FLOW(("q depth %d, decoded %d, displayed %d, decode errs %d, display errs %d, decode drops %d, display drops %d, display underflow %d, received %d, pts errs %d\n",
                    status.queueDepth, status.numDecoded, status.numDisplayed, status.numDecodeErrors,
                    status.numDisplayErrors, status.numDecodeDrops, status.numDisplayDrops, status.numDisplayUnderflows, status.numPicturesReceived, status.ptsErrorCount));
    }
#endif
	
    /* for Audio only stream, get the PTS of the last decoded Sample */
    else if (playback_ip->nexusHandles.primaryAudioDecoder) {
        NEXUS_AudioDecoderStatus audioStatus;
        if ((NEXUS_AudioDecoder_GetStatus(playback_ip->nexusHandles.primaryAudioDecoder, &audioStatus) != NEXUS_SUCCESS) || (audioStatus.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS)) {
            BDBG_ERR(("%s: %s", __FUNCTION__, (audioStatus.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS) ? "Got NEXUS_PtsType_eInterpolatedFromInvalidPTS, ignore it":"Failed to get current video PTS"));
            return B_ERROR_UNKNOWN;
        }
        *currentPts = audioStatus.pts;
    }
    else if (playback_ip->nexusHandles.secondaryAudioDecoder) {
        NEXUS_AudioDecoderStatus audioStatus;
        if ((NEXUS_AudioDecoder_GetStatus(playback_ip->nexusHandles.secondaryAudioDecoder, &audioStatus) != NEXUS_SUCCESS) || (audioStatus.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS)) {
            BDBG_ERR(("%s: %s", __FUNCTION__, (audioStatus.ptsType == NEXUS_PtsType_eInterpolatedFromInvalidPTS) ? "Got NEXUS_PtsType_eInterpolatedFromInvalidPTS, ignore it":"Failed to get current video PTS"));
            return B_ERROR_UNKNOWN;
        }
        *currentPts = audioStatus.pts;
    }
    else {
        /* if no other choice, just re-start from begining */
        *currentPts = 0;
    }
    BDBG_MSG_FLOW(("%s: current PTS %lu", __FUNCTION__, *currentPts));
    return B_ERROR_SUCCESS;
}

/* this function computes the current position by taking into account PTS wraps and discontinuities */
NEXUS_PlaybackPosition 
http_calculate_current_position(B_PlaybackIpHandle playback_ip, unsigned currentPts)
{
    NEXUS_PlaybackPosition currentPosition = 0;

    if (!playback_ip->mediaStartTimeNoted)
        /* we haven't yet received 1st pts callback and thus not noted the media start time, so we return time we have accumulated till last discontinuity */
        /* this can happen either during seek (when decoders are flushed) or when sync channel has to flush the decoders (as they are full due to TSM not passing, frames in future due to lost frames) */
        return (playback_ip->streamDurationUntilLastDiscontinuity);

    if (currentPts == 0) {
        /* current PTS is 0 (as DM may have been reset during channel change or DM may have been underflowed (which looks like a bug in DM) */
        BDBG_MSG(("%s: currentPts is 0, using lastPts as currentPts, lastPts %x, firstPts %x", __FUNCTION__, playback_ip->lastUsedPts, playback_ip->firstPts));
        if (playback_ip->getNewPtsAfterDiscontinuity) {
            /* there was a recent pts discontinuity event and thus we needed to get the new currentPts, but we haven't gotten one yet */
            /* so we use the firstPts before the discontinuity in our calculation */
            currentPosition = (playback_ip->lastUsedPts - playback_ip->firstPtsBeforePrevDiscontinuity) / 45; /* in msec: our decoders report PTS in 45Khz rate */
        }
        else {
            currentPosition = (playback_ip->lastUsedPts - playback_ip->firstPts) / 45; /* in msec: our decoders report PTS in 45Khz rate */
        }
        currentPosition += playback_ip->streamDurationUntilLastDiscontinuity;
    }
    else if (playback_ip->getNewPtsAfterDiscontinuity && currentPts == playback_ip->lastUsedPts) {
        /* we had a pts discontinuity, and thus needed to get the new currentPts, */ 
        /* but the currentPts happens to be same as the lastPts */
        /* This can happen when TSM hasn't yet matured for the new frames after the discontinuity */
        /* so we use the previous firstPts to calculate the current position so far */
        BDBG_MSG(("%s: currentPts hasn't changed since the last discontinuity, lastPts %x, currentPts %x", __FUNCTION__, playback_ip->lastUsedPts, currentPts));
        currentPosition = (playback_ip->lastUsedPts - playback_ip->firstPtsBeforePrevDiscontinuity) / 45; /* in msec: our decoders report PTS in 45Khz rate */
        currentPosition = playback_ip->streamDurationUntilLastDiscontinuity;
    }
    else {
        /* current PTS is valid, current - first PTS gives us the current position (+ streamDurationUntilLastDiscontinuity) */
        /* since these variables are unsigned, unsigned math takes care of the PTS wrap logic as well */
        currentPosition = (currentPts - playback_ip->firstPts) / 45; /* in msec: our decoders report PTS in 45Khz rate */
        currentPosition += playback_ip->streamDurationUntilLastDiscontinuity;
        playback_ip->getNewPtsAfterDiscontinuity = false; /* as we successfully got the new current pts */
        playback_ip->lastUsedPts = currentPts;
        playback_ip->ptsDuringLastDiscontinuity = 0;
        BDBG_MSG(("%s: currentPts %x, firstPts %x, lastPts %x, pos %u streamDurationUntilLastDiscontinuity %u", __FUNCTION__, currentPts, playback_ip->firstPts, playback_ip->lastUsedPts, currentPosition, playback_ip->streamDurationUntilLastDiscontinuity));
    }
    return currentPosition;
}

B_PlaybackIpError 
B_PlaybackIp_HttpGetCurrentPlaybackPosition(
        B_PlaybackIpHandle playback_ip,
        NEXUS_PlaybackPosition *currentPosition
        )
{
    unsigned currentPts = 0;
    NEXUS_PlaybackStatus playbackStatus;

    if (playback_ip->useNexusPlaypump) {
        if (playback_ip->lastSeekPositionSet) {
            /* a seek operation is currently in progress but we haven't yet received the 1st frame after seeking */
            /* so currentPosition calculation using current PTS will not show the new seeked position. */
            /* To take care of the case, where seek was invoked because user has slided the timebar, sending older timestamp */
            /* will cause the timebar to jump backwards. To take care of this case, we use the lastSeekPosition as the current one */
            *currentPosition = playback_ip->lastSeekPosition;
        }
        else {
            if (http_get_current_pts(playback_ip, &currentPts) != B_ERROR_SUCCESS) {
                /* if there is an error in getting the current PTS value, we still do the position calculation assuming it is 0 */
                /* which will basically use the last valid PTS value in the http_calculate_current_position */
                currentPts = 0;
            }
            *currentPosition = http_calculate_current_position(playback_ip, currentPts); /* accounts for check for PTS wrap & discontinuity */
        }
        BDBG_MSG(("%s: current position at %0.3f sec", __FUNCTION__, *currentPosition/1000.));
        playback_ip->pausedByteOffset = *currentPosition * (playback_ip->psi.avgBitRate/8000.0); /* byte position * byte rate in milli-sec */
    }
    else {
        /* not using playpump, so continue below to get position from Nexus playback */
        if (playback_ip->nexusHandles.playback) {
            if (NEXUS_Playback_GetStatus(playback_ip->nexusHandles.playback, &playbackStatus) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: ERROR: NEXUS_Playback_GetStatus() Failed\n", __FUNCTION__));
                return B_ERROR_UNKNOWN;
            }
            /* this is the position of the last frame that was sent to the vdec */
            *currentPosition = playbackStatus.position;
        }
        else 
            *currentPosition = 0;
        playback_ip->pausedByteOffset = *currentPosition * (playback_ip->psi.avgBitRate/8000.0); /* byte position * byte rate in milli-sec */
        if (playback_ip->psi.duration && *currentPosition >= playback_ip->psi.duration) {
            BDBG_MSG(("%s: calculated incorrect current position (%u) as is outside the file duration %d, setting it to file duration\n", __FUNCTION__, *currentPosition, playback_ip->psi.duration));
            *currentPosition = playback_ip->psi.duration;
        }
        BDBG_MSG(("%s: current position: at %0.3f sec, byte offset %lld", __FUNCTION__, *currentPosition/1000., playback_ip->pausedByteOffset));
    }
    return B_ERROR_SUCCESS;
}

#ifdef NEXUS_HAS_SIMPLE_DECODER
static B_PlaybackIpError 
updateSimpleNexusAudioDecodersState(
    B_PlaybackIpHandle playback_ip, 
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings,
    NEXUS_SimpleAudioDecoderHandle simpleAudioDecoder
    )
{
    B_PlaybackIpError rc;
    NEXUS_AudioDecoderTrickState audioTrickSettings;

    NEXUS_SimpleAudioDecoder_GetTrickState(simpleAudioDecoder, &audioTrickSettings);
    audioTrickSettings.forceStopped = (ipTrickModeSettings->rate == 1 || playback_ip->stcTrickMode) ?
        false :  /* for normal play, re-enable the decoder */
        true;  /* for trick modes, stop the audio decoder */
    if (playback_ip->stcTrickMode)
        audioTrickSettings.rate = playback_ip->stcRate;
    else if (ipTrickModeSettings->frameRepeat)
        audioTrickSettings.rate = NEXUS_NORMAL_DECODE_RATE/ipTrickModeSettings->frameRepeat;
    else if (ipTrickModeSettings->rate == 0)
        audioTrickSettings.rate = 0;
    else
        audioTrickSettings.rate = NEXUS_NORMAL_DECODE_RATE;
    audioTrickSettings.muted = (ipTrickModeSettings->rate == 1 && !playback_ip->stcTrickMode) ? 
        false :  /* for normal play, dont mute */
        true;  /* for trick modes (mainly for STC trickmodes), mute the audio decoder */
    if (NEXUS_SimpleAudioDecoder_SetTrickState(simpleAudioDecoder, &audioTrickSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: NEXUS_AudioDecoder_SetTrickState() failed for primary audio decoder \n", __FUNCTION__));
        rc = B_ERROR_UNKNOWN;
    }
    else 
        rc = B_ERROR_SUCCESS;

    return rc;
}
#endif

static B_PlaybackIpError 
updateNexusAudioDecodersState(
    B_PlaybackIpHandle playback_ip, 
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings,
    NEXUS_AudioDecoderHandle audioDecoder
    )
{
    B_PlaybackIpError rc;
    NEXUS_AudioDecoderTrickState audioTrickSettings;

    NEXUS_AudioDecoder_GetTrickState(audioDecoder, &audioTrickSettings);
    audioTrickSettings.forceStopped = (ipTrickModeSettings->rate == 0 || ipTrickModeSettings->rate == 1 || playback_ip->stcTrickMode) ?
        false :  /* for normal play, re-enable the decoder */
        true;  /* for trick modes, stop the audio decoder */
    if (playback_ip->stcTrickMode)
        audioTrickSettings.rate = playback_ip->stcRate;
    else if (ipTrickModeSettings->frameRepeat)
        audioTrickSettings.rate = NEXUS_NORMAL_DECODE_RATE/ipTrickModeSettings->frameRepeat;
    else if (ipTrickModeSettings->rate == 0)
        audioTrickSettings.rate = 0;
    else
        audioTrickSettings.rate = NEXUS_NORMAL_DECODE_RATE;
    audioTrickSettings.muted = (ipTrickModeSettings->rate == 1 && !playback_ip->stcTrickMode) ? 
        false :  /* for normal play, dont mute */
        true;  /* for trick modes (mainly for STC trickmodes), mute the audio decoder */
    if (NEXUS_AudioDecoder_SetTrickState(audioDecoder, &audioTrickSettings) != NEXUS_SUCCESS) {
        BDBG_ERR(("%s: NEXUS_AudioDecoder_SetTrickState() failed for primary audio decoder \n", __FUNCTION__));
        rc = B_ERROR_UNKNOWN;
    }
    else 
        rc = B_ERROR_SUCCESS;

    return rc;
}

static int 
mapNexusFrameRateEnumToNum(NEXUS_VideoFrameRate frameRate)
{
    int frameRateNum;
    switch (frameRate)
    {
        case NEXUS_VideoFrameRate_e23_976:
        case NEXUS_VideoFrameRate_e24:
            frameRateNum = 24;
            break;
        case NEXUS_VideoFrameRate_e25:
            frameRateNum = 25;
            break;
        case NEXUS_VideoFrameRate_e29_97:
        case NEXUS_VideoFrameRate_e30:
            frameRateNum = 30;
            break;
        case NEXUS_VideoFrameRate_e50:
            frameRateNum = 50;
            break;
        case NEXUS_VideoFrameRate_e59_94:
        case NEXUS_VideoFrameRate_e60:
            frameRateNum = 60;
            break;
        case NEXUS_VideoFrameRate_e14_985:
            frameRateNum = 15;
            break;
        case NEXUS_VideoFrameRate_e7_493:
            frameRateNum = 7;
            break;
        default:
            /* not specified, we default frame rate to 30 */
            frameRateNum = 30;
    }
    BDBG_MSG(("frame rate %d, frameRateNum %d", frameRate, frameRateNum));
    return frameRateNum;
}

static B_PlaybackIpError 
updateNexusPlaypumpDecodersState(
    B_PlaybackIpHandle playback_ip, 
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    bool pausePlaypump;
    NEXUS_VideoDecoderTrickState videoDecoderTrickSettings;

    BDBG_MSG(("%s: rate %d, frame repeat %d", __FUNCTION__, ipTrickModeSettings->rate, ipTrickModeSettings->frameRepeat));

    if (playback_ip->nexusHandles.stcChannel) {
        if (!playback_ip->stcTrickMode)
            playback_ip->stcRate = NEXUS_NORMAL_PLAY_SPEED;
        if (NEXUS_StcChannel_SetRate(playback_ip->nexusHandles.stcChannel, playback_ip->stcRate, NEXUS_NORMAL_PLAY_SPEED-1/*HW adds one*/) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: NEXUS_StcChannel_SetRate() failed \n", __FUNCTION__));
            return B_ERROR_UNKNOWN;
        }
        BDBG_MSG(("%s: STC Trickmode %s, STC Rate %d", __FUNCTION__, playback_ip->stcTrickMode ? "Y":"N", playback_ip->stcRate));

        /* Invalidate STC channel to force the STC to get reloaded. This is required to minimize the loss of */
        /* audio/video data when transitioning from decoder_pause-->play or decoder_pause->seek->play. */
        /* The problem shows up if the time period between transition is long. */
        if (NEXUS_StcChannel_Invalidate(playback_ip->nexusHandles.stcChannel) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: NEXUS_StcChannel_Invalidate() failed \n", __FUNCTION__));
            return B_ERROR_UNKNOWN;
        }
    }

    /* update audio decoder state */
#ifdef NEXUS_HAS_SIMPLE_DECODER
    if (playback_ip->nexusHandles.simpleAudioDecoder) {
        if (updateSimpleNexusAudioDecodersState(playback_ip, ipTrickModeSettings, playback_ip->nexusHandles.simpleAudioDecoder) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during trickmode transition\n", __FUNCTION__));
            return B_ERROR_UNKNOWN;
        }
    }
    else 
#endif
    {
       if (playback_ip->nexusHandles.primaryAudioDecoder) {
           if (updateNexusAudioDecodersState(playback_ip, ipTrickModeSettings, playback_ip->nexusHandles.primaryAudioDecoder) != B_ERROR_SUCCESS) {
               BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during trickmode transition\n", __FUNCTION__));
               return B_ERROR_UNKNOWN;
           }
       }
       
       if (playback_ip->nexusHandles.secondaryAudioDecoder) {
           if (updateNexusAudioDecodersState(playback_ip, ipTrickModeSettings, playback_ip->nexusHandles.secondaryAudioDecoder) != B_ERROR_SUCCESS) {
               BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during trickmode transition\n", __FUNCTION__));
               return B_ERROR_UNKNOWN;
           }
       }
    }

    
    if (playback_ip->stcTrickMode) {
        /* for STC trickmodes, no need to update video decoder trick state, it is driven by controlling the STC speed */
        BDBG_MSG(("%s: Trick play settings are sucessfully applied to Nexus Audio decoder and Stc", __FUNCTION__));
        return B_ERROR_SUCCESS;
    }

    /* update Nexus Playpump state */
    if (playback_ip->nexusHandles.playpump) {
        bool suspendPacing = false;
        if (ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UseRvuSpec) {
            suspendPacing = ipTrickModeSettings->rate == 1? false:true;
            if (B_PlaybackIp_TtsThrottle_SuspendPacing(playback_ip->ttsThrottle, suspendPacing) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: B_PlaybackIp_TtsThrottle_SuspendPacing() Failed to %s", __FUNCTION__, suspendPacing ? "suspend pacing":"resume pacing"));
                return B_ERROR_UNKNOWN;
            }
        }
        if (ipTrickModeSettings->rate == 0) 
            pausePlaypump = true;
        else
            pausePlaypump = false;
        if (NEXUS_Playpump_SetPause(playback_ip->nexusHandles.playpump, pausePlaypump) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: NEXUS_Playpump_SetPause() failed to %s", __FUNCTION__, pausePlaypump ? "set pause":"unset pause"));
            return B_ERROR_UNKNOWN;
        }
        BDBG_MSG(("%s: playpump pause %s, suspend pacing %s", __FUNCTION__, pausePlaypump ? "Y":"N", suspendPacing ? "Y":"N"));
    }

    /* update video decoder settings for streams playing video */
    if (playback_ip->nexusHandles.videoDecoder || playback_ip->nexusHandles.simpleVideoDecoder) {
        if (playback_ip->nexusHandles.videoDecoder)
            NEXUS_VideoDecoder_GetTrickState(playback_ip->nexusHandles.videoDecoder, &videoDecoderTrickSettings);
#ifdef NEXUS_HAS_SIMPLE_DECODER
        else
            NEXUS_SimpleVideoDecoder_GetTrickState(playback_ip->nexusHandles.simpleVideoDecoder, &videoDecoderTrickSettings);
#endif

        /* this flag enables decoder/display manager to decode/display each frame as fast as it can w/o waiting for its time maturity */
        videoDecoderTrickSettings.tsmEnabled = (ipTrickModeSettings->rate == 1 || ipTrickModeSettings->rate == 2) ?
            true :  /* for normal play, re-enable the TSM */
            false;  /* for trick modes, turn-off the TSM */

        /* decode pictures as requested by the app */
        if (ipTrickModeSettings->rate == 1 || playback_ip->stcTrickMode) {
            videoDecoderTrickSettings.decodeMode = ipTrickModeSettings->decodeMode;
        }
        else {
            /* client decoder is put in the iframe only mode as server will only send i-frames in the trickmodes */
            videoDecoderTrickSettings.decodeMode = NEXUS_VideoDecoderDecodeMode_eI;
        }

        videoDecoderTrickSettings.hostTrickModesEnabled = (ipTrickModeSettings->rate == 1 || ipTrickModeSettings->rate == 0 || ipTrickModeSettings->rate == 2) ?
            false :  /* for normal play/pause, no host trick modes are enabled */
            true;  /* for trick modes, we are using host trick modes as server is manipulating the streams */

        if (ipTrickModeSettings->rate == 0)
            videoDecoderTrickSettings.rate = 0; /* pause */
        /* set the decoder rate to slow it down, typically done at higher speeds when displaying only I-frames */
        else if (ipTrickModeSettings->rate != 1 && playback_ip->frameRate != 0 && (playback_ip->psi.frameRateInTrickMode || ipTrickModeSettings->frameRateInTrickMode)) {
            int frameRateNum;
            int frameRateInTrickMode;
            frameRateInTrickMode = ipTrickModeSettings->frameRateInTrickMode ? ipTrickModeSettings->frameRateInTrickMode : playback_ip->psi.frameRateInTrickMode;
            frameRateNum = mapNexusFrameRateEnumToNum(playback_ip->frameRate);
            BDBG_MSG(("%s: frame rate %d, nexus enum %d , frameRateInTrickMode %d", __FUNCTION__, frameRateNum, playback_ip->frameRate, frameRateInTrickMode));
                videoDecoderTrickSettings.rate = (NEXUS_NORMAL_DECODE_RATE/frameRateNum)*frameRateInTrickMode;
        }
        else if (ipTrickModeSettings->frameRepeat) {
            videoDecoderTrickSettings.rate = NEXUS_NORMAL_DECODE_RATE/ipTrickModeSettings->frameRepeat;
        }
        else
            videoDecoderTrickSettings.rate = NEXUS_NORMAL_DECODE_RATE;
        /* for interlaced content, tell decoder to send only the top field to display */
        videoDecoderTrickSettings.topFieldOnly = (ipTrickModeSettings->rate == 1 || ipTrickModeSettings->rate == 2) ?
            false :  /* for normal play, send both top & bottom fields */
            true;  /* for trick modes, send only the top field */
        /* for fast reverse, set reverseFields flag to true */
        if (ipTrickModeSettings->rate < 0)
            videoDecoderTrickSettings.reverseFields = true;
        else
            videoDecoderTrickSettings.reverseFields = false;
        if (playback_ip->nexusHandles.videoDecoder) {
            if (NEXUS_VideoDecoder_SetTrickState(playback_ip->nexusHandles.videoDecoder, &videoDecoderTrickSettings) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_VideoDecoder_SetTrickState() failed \n", __FUNCTION__));
                return B_ERROR_UNKNOWN;
            }
        }
#ifdef NEXUS_HAS_SIMPLE_DECODER
        else {
            if (NEXUS_SimpleVideoDecoder_SetTrickState(playback_ip->nexusHandles.simpleVideoDecoder, &videoDecoderTrickSettings) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_VideoDecoder_SetTrickState() failed \n", __FUNCTION__));
                return B_ERROR_UNKNOWN;
            }
        }
#endif

#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            BDBG_WRN(("Video Decoder Settings: Host %s, TSM %s, Rate %d, Top %s, Rev %s",
                    videoDecoderTrickSettings.hostTrickModesEnabled? "Y":"N",
                    videoDecoderTrickSettings.tsmEnabled? "Y":"N",
                    videoDecoderTrickSettings.rate,
                    videoDecoderTrickSettings.topFieldOnly? "Y":"N",
                    videoDecoderTrickSettings.reverseFields? "Y":"N"
                 ));
#endif
    }

    BDBG_MSG(("%s: Trick play settings are sucessfully applied to Nexus AV decoders and Playpump", __FUNCTION__));
    return B_ERROR_SUCCESS;
}

B_PlaybackIpError 
http_send_time_seek_request(
    B_PlaybackIpHandle playback_ip
    )
{
    B_PlaybackIpError rc;
    double timeSeekRangeBegin;
    double timeSeekRangeEnd;

    if (playback_ip->ipTrickModeSettings.rate > 0) {
        timeSeekRangeBegin = playback_ip->initialTimeSeekRangeBegin;
        timeSeekRangeEnd = timeSeekRangeBegin + playback_ip->timeSeekRangeInterval;
        playback_ip->initialTimeSeekRangeBegin = timeSeekRangeEnd; 
    }
    else {
        timeSeekRangeEnd = playback_ip->initialTimeSeekRangeBegin;
        timeSeekRangeBegin = timeSeekRangeEnd - playback_ip->timeSeekRangeInterval;
        if (timeSeekRangeBegin < 0)
            timeSeekRangeBegin = 0.;
        playback_ip->initialTimeSeekRangeBegin = timeSeekRangeBegin; 
    }

    BDBG_MSG(("%s: initTimeSeekRangeBegin %0.3f, interval %0.3f, timeSeekRangeBegin %0.3f, timeSeekRangeEnd %0.3f, state %d", 
                __FUNCTION__, playback_ip->initialTimeSeekRangeBegin, playback_ip->timeSeekRangeInterval, timeSeekRangeBegin, timeSeekRangeEnd, playback_ip->playback_state));

    /* now build & send the new get request so that server will send stream at new speed */
    if ((rc = http_do_server_trick_modes_socket_transition(playback_ip, timeSeekRangeBegin, timeSeekRangeEnd, playback_ip->ipTrickModeSettings.rate)) != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s: Failed to update server about the trick play transition\n", __FUNCTION__));
        return B_ERROR_UNKNOWN;
    }

    BDBG_ERR(("%s: Done", __FUNCTION__));
    return B_ERROR_SUCCESS;
}

static B_PlaybackIpError 
http_do_client_side_trick_modes_using_playback(
        B_PlaybackIpHandle playback_ip, 
        B_PlaybackIpTrickModesSettings *ipTrickModeSettings
        )
{
    NEXUS_PlaybackTrickModeSettings playbackTrickModeSettings;
    NEXUS_Playback_GetDefaultTrickModeSettings(&playbackTrickModeSettings);
    int sign;
    int rate;

    playbackTrickModeSettings.mode = NEXUS_PlaybackHostTrickMode_eNone; /* this allows rate to take affect */
    if (ipTrickModeSettings->absoluteRateDefined) {
        playbackTrickModeSettings.rate = ipTrickModeSettings->absoluteRate;
    }
    else {
        if (ipTrickModeSettings->rate < 0) {
            sign = -1;
            rate = sign * ipTrickModeSettings->rate;
        }
        else {
            sign = 1;
            rate = ipTrickModeSettings->rate;
        }
        switch (rate) {
        case 1:
            playbackTrickModeSettings.rate = sign*NEXUS_NORMAL_PLAY_SPEED;
            break;
        case 2:
            playbackTrickModeSettings.rate = sign*2*NEXUS_NORMAL_PLAY_SPEED;
            break;
        case 3:
            playbackTrickModeSettings.rate = sign*5*NEXUS_NORMAL_PLAY_SPEED;
            break;
        case 4:
            playbackTrickModeSettings.rate = sign*10*NEXUS_NORMAL_PLAY_SPEED;
            break;
        case 5:
            playbackTrickModeSettings.rate = sign*25*NEXUS_NORMAL_PLAY_SPEED;
            break;
        case 6:
            playbackTrickModeSettings.rate = sign*50*NEXUS_NORMAL_PLAY_SPEED;
            break;
        case 7:
            playbackTrickModeSettings.rate = sign*100*NEXUS_NORMAL_PLAY_SPEED;
            break;
        default :
            playbackTrickModeSettings.rate = sign*200*NEXUS_NORMAL_PLAY_SPEED;
            break;
        }
    }

    BDBG_MSG(("%s: Calling NEXUS_Playback_TrickMode() nexus rate %d", __FUNCTION__, playbackTrickModeSettings.rate));
    if (NEXUS_Playback_TrickMode(playback_ip->nexusHandles.playback, &playbackTrickModeSettings) != NEXUS_SUCCESS) {
        BDBG_WRN(("%s: NEXUS_Playback_TrickMode() failed for rate %d\n", __FUNCTION__, ipTrickModeSettings->rate));
        return (B_ERROR_UNKNOWN);
    }

    BDBG_MSG(("%s: NEXUS_Playback_TrickMode() successfully updated the trick mode rate to %d, nexus rate %d", __FUNCTION__, ipTrickModeSettings->rate, playbackTrickModeSettings.rate));
    return B_ERROR_SUCCESS;
}

void B_PlaybackIp_HttpSetDefaultTrickModeSettings(
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    memset(ipTrickModeSettings, 0, sizeof(B_PlaybackIpTrickModesSettings));
    ipTrickModeSettings->method = B_PlaybackIpTrickModesMethod_UseByteRange;    /* default to client side trickmodes */
    ipTrickModeSettings->rate = 1;  /* normal rate unless app sets it otherwise */
    ipTrickModeSettings->frameRepeat = 1;
    ipTrickModeSettings->pauseMethod = B_PlaybackIpPauseMethod_UseDisconnectAndSeek;
    ipTrickModeSettings->seekPosition = 0;  /* beginging of stream */
    ipTrickModeSettings->seekPositionIsRelative = false;  /* assume all seeks positions are absolute unless app specifically sets this flag */
    ipTrickModeSettings->seekBackward = false;  /* assume all relative seeks are in forward direction unless app specifically sets this flag */
    ipTrickModeSettings->decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;  /* assume all relative seeks are in forward direction unless app specifically sets this flag */
    return;
}

B_PlaybackIpError 
B_PlaybackIp_GetTrickModeSettings(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    if (!playback_ip || !ipTrickModeSettings) {
        BDBG_ERR(("%s: ERROR: incorrect params: playback_ip %p, trickMode %p\n", __FUNCTION__, playback_ip, ipTrickModeSettings));
        return B_ERROR_INVALID_PARAMETER;
    }
    B_PlaybackIp_HttpSetDefaultTrickModeSettings(&playback_ip->ipTrickModeSettings);
    *ipTrickModeSettings = playback_ip->ipTrickModeSettings;
    if (playback_ip->useNexusPlaypump)
        ipTrickModeSettings->method = B_PlaybackIpTrickModesMethod_UsePlaySpeed;
    return B_ERROR_SUCCESS;
}

void unlock_ip_session(
    B_PlaybackIpHandle playback_ip
    )
{
    BKNI_ReleaseMutex(playback_ip->lock);
}

B_PlaybackIpError lock_ip_session(
    B_PlaybackIpHandle playback_ip
    )
{
    B_PlaybackIpError rc = B_ERROR_SUCCESS;
    B_PlaybackIpState currentState;

    /* set state to waiting to enter trickmodes, this allows Nexus IO & HTTP thread to finish its work ASAP and release the mutex */
    currentState = playback_ip->playback_state;
    playback_ip->playback_state = B_PlaybackIpState_eWaitingToEnterTrickMode;

    /* now wait the lock to be released by other threads (Nexus IO or HTTP main thread) */
    if (BKNI_AcquireMutex(playback_ip->lock) != B_ERROR_SUCCESS) {
        rc = B_ERROR_UNKNOWN;
    }
    else {
        rc = B_ERROR_SUCCESS;
    }

    /* now restore the current state */
    playback_ip->playback_state = currentState;

    return rc;
}

B_PlaybackIpError 
B_PlaybackIp_PauseRvu(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    BSTD_UNUSED(playback_ip);
    BSTD_UNUSED(ipTrickModeSettings);
    BDBG_ERR(("%s: ERROR: Rvu Pause is not yet supported", __FUNCTION__));
    return B_ERROR_SUCCESS;
}

#ifdef LIVEMEDIA_SUPPORT
B_PlaybackIpError 
B_PlaybackIp_PauseRtsp(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    B_PlaybackIpError rc;
    B_PlaybackIp_RtspCtrlCmd cmd;

    /* send Pause command to the server */
    cmd.type = B_PlaybackIp_RtspCtrlCmdType_Pause;
    B_PlaybackIp_liveMediaSessionRTSPControl(playback_ip->lm_context, &cmd);

    /* disable the live mode so that feeder thread doesn't detect any discontinuities introduced due to longer n/w latency */
    playback_ip->disableLiveMode = true;

    /* update the decoder state */
    ipTrickModeSettings->rate = 0;
    ipTrickModeSettings->frameRepeat = 0;
    ipTrickModeSettings->decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
    if ((rc = updateNexusPlaypumpDecodersState(playback_ip, ipTrickModeSettings)) != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during pause transition\n", __FUNCTION__));
        return rc;
    }
    return B_ERROR_SUCCESS;
}
#endif /* LIVEMEDIA_SUPPORT */

/* handles pause for non-HLS, non-RVU HTTP sessions: includes sessions from other DLNA Servers */
/* For such HTTP session, either Nexus Playpump or Playback can be used so we have to be careful in choosing the correct interface */
/* Plus, we have to consider the pause method that app wants us to use: connx stalling or disconnect & seek */
B_PlaybackIpError 
B_PlaybackIp_PauseHttp(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    B_PlaybackIpError rc = B_ERROR_SUCCESS;

    if (!playback_ip->useNexusPlaypump) {
        /* pause Nexus Playback and it takes care of pausing decoders & playpump */
        if (NEXUS_Playback_Pause(playback_ip->nexusHandles.playback) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: Failed to Pause Nexus Playback"));
            rc = B_ERROR_UNKNOWN;
            goto error;
        }
    }
    else {
        /* using Nexus Playpump, so we will need to inform both Decoders & Playpump about trickplay */
        ipTrickModeSettings->rate = 0;
        ipTrickModeSettings->frameRepeat = 0;
        ipTrickModeSettings->decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
        if ((rc = updateNexusPlaypumpDecodersState(playback_ip, ipTrickModeSettings)) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during pause transition", __FUNCTION__));
            goto error;
        }
    }

    if (!playback_ip->psi.liveChannel && ipTrickModeSettings->pauseMethod == B_PlaybackIpPauseMethod_UseDisconnectAndSeek) {
        /* if app is telling us to pause using disconnect & seek method, then just pausing nexus playback/playpump is NOT enough. */
        /* in addition, we need to close the current socket and store some state to resume playback later from this point */

        /* do the ground work to enable cloning the security handle */
        /* if security layer has defined the shutdown callback, we call it to start the shutdown of the security handle */
        /* this basically tells security module to free up an security context w/o destroying the ability to clone it */
        if (playback_ip->netIo.shutdown && !playback_ip->serverClosed)
            playback_ip->netIo.shutdown(playback_ip->securityHandle, 0);

        /* now disconnect: close the current socket to meet DLNA requirement of pause using disconnecting socket */
        close(playback_ip->socketState.fd);
        if (!playback_ip->serverClosed) {
            /* since server hasn't closed (meaning it hasn't yet sent the full content), set a flag to re-open the socket */
            playback_ip->reOpenSocket = true; /* this flags tells HTTP layer to reopen the connection */
            playback_ip->reOpenIndexSocket = true; /* this flags tells HTTP layer to reopen the connection if index reading flow is continued */
        }
        /* set a flag to prevent this socket from being closed again in NetRange function */
        playback_ip->socketClosed = true; 
        BDBG_MSG(("%s: Paused Nexus & IP Playback using DisconnectAndSeek Method (closed socket)", __FUNCTION__));
    }
    else {
        /* if app is telling us to pause using connection stalling, or we are pausing a live channel */
        /* then just pausing nexus playback/playpump is enough. */
        /* Since decoders are in pause state, this will eventually cause all buffers in pipleline to fill up and */
        /* thus read io thread will stop reading from the socket. This will cause TCP buffers to fill and thus */
        /* TCP flow control will send 0 TCP window to server and thus make it stop sending the content */
        BDBG_MSG(("%s: Paused IP Playback using Connection Stalling Method", __FUNCTION__));
    }
    rc = B_ERROR_SUCCESS;
    return rc;

error:
    return rc;
}

B_PlaybackIpError 
B_PlaybackIp_Pause(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    B_PlaybackIpError rc = B_ERROR_SUCCESS;
    B_PlaybackIpState currentState;
    B_Time currentTime;

    if (!playback_ip || !ipTrickModeSettings) {
        BDBG_ERR(("%s: ERROR: incorrect params: playback_ip %p, trickMode %p", __FUNCTION__, playback_ip, ipTrickModeSettings));
        return B_ERROR_INVALID_PARAMETER;
    }

    BDBG_MSG(("%s: Pausing playback_ip %p, method %d", __FUNCTION__, playback_ip, ipTrickModeSettings->pauseMethod, playback_ip->socketState.fd));
    if (playback_ip->playback_state != B_PlaybackIpState_ePaused && 
            playback_ip->playback_state != B_PlaybackIpState_ePlaying && 
            playback_ip->playback_state != B_PlaybackIpState_eTrickMode) {
        BDBG_ERR(("%s: ERROR: Can't do Pause command in this state %d", __FUNCTION__, playback_ip->playback_state));
        return B_ERROR_INVALID_PARAMETER;
    }

    if (playback_ip->playback_state == B_PlaybackIpState_ePaused) {
        /* already in pause state, so resume */
        if (B_PlaybackIp_Play(playback_ip)) {
            BDBG_ERR(("%s: ERROR: Failed to resume from pause", __FUNCTION__));
            rc = B_ERROR_UNKNOWN;
            return rc;
        }
        BDBG_MSG(("%s: resuming from pause", __FUNCTION__));
        return B_ERROR_SUCCESS;
    }

    B_Time_Get(&currentTime);
    BDBG_MSG(("%s: time played %d until paused", __FUNCTION__, B_Time_Diff(&currentTime, &playback_ip->mediaStartTime)));

    /* lock the playback session */
    lock_ip_session(playback_ip);

    /* now change the state to EnteringTrickMode until we finish the trickmode work */
    currentState = playback_ip->playback_state;
    playback_ip->playback_state = B_PlaybackIpState_eEnteringTrickMode;

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    if (playback_ip->hlsSessionEnabled) 
        rc = B_PlaybackIp_PauseHls(playback_ip, ipTrickModeSettings);
    else
#endif
    if (playback_ip->protocol == B_PlaybackIpProtocol_eHttp)
        rc = B_PlaybackIp_PauseHttp(playback_ip, ipTrickModeSettings);
#ifdef LIVEMEDIA_SUPPORT
    else if (playback_ip->protocol == B_PlaybackIpProtocol_eRtsp)
        rc = B_PlaybackIp_PauseRtsp(playback_ip, ipTrickModeSettings);
#endif
    else if (ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UseRvuSpec)
        rc = B_PlaybackIp_PauseRvu(playback_ip, ipTrickModeSettings);
    else {
        BDBG_ERR(("%s: ERROR: Pause not supported for IP protocol %d", __FUNCTION__, playback_ip->protocol));
        rc = B_ERROR_NOT_SUPPORTED;
    }

    if (rc == B_ERROR_SUCCESS) {
        playback_ip->playback_state = B_PlaybackIpState_ePaused;
        playback_ip->ipTrickModeSettings = *ipTrickModeSettings;
        BDBG_MSG(("%s: Successfully Paused IP Playback, ip state %d", __FUNCTION__, playback_ip->playback_state));
    }
    else {
        /* in case of error, reset the state back to current state */
        playback_ip->playback_state = currentState;
    }
    unlock_ip_session(playback_ip);
    return rc;
}

#ifdef LIVEMEDIA_SUPPORT
B_PlaybackIpError 
B_PlaybackIp_PlayRtsp(
    B_PlaybackIpHandle playback_ip
    ) 
{
    B_PlaybackIpError rc;
    B_PlaybackIpTrickModesSettings ipTrickModeSettings;
    B_PlaybackIp_RtspCtrlCmd cmd;

    /* send resume command to RTSP server */
    cmd.type = B_PlaybackIp_RtspCtrlCmdType_Resume;
    /* (Note: start=-1 means 'resume'; end=-1 means 'play to end') */
    cmd.params.end = -1;
    cmd.params.scale = 1;
    B_PlaybackIp_liveMediaSessionRTSPControl(playback_ip->lm_context, &cmd);

    /* re-enable the live mode so that feeder thread can detect any discontinuities introduced due to longer n/w latency */
    playback_ip->disableLiveMode = false;

    /* program decoders to resume normal play */
    ipTrickModeSettings.rate = 1;
    ipTrickModeSettings.frameRepeat = 0;
    ipTrickModeSettings.decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
    if ((rc = updateNexusPlaypumpDecodersState(playback_ip, &ipTrickModeSettings)) != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during trickmode transition\n", __FUNCTION__));
        return rc;
    }
    return B_ERROR_SUCCESS;
}
#endif /* LIVEMEDIA_SUPPORT */

B_PlaybackIpError 
B_PlaybackIp_PlayHttp(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpState currentState
    ) 
{
    B_PlaybackIpError rc = B_ERROR_UNKNOWN;
    B_PlaybackIpTrickModesSettings ipTrickModeSettings = 
	{	B_PlaybackIpTrickModesMethod_UseByteRange, 1, false, NEXUS_NORMAL_DECODE_RATE, 1, 0, 
		B_PlaybackIpPauseMethod_UseConnectionStalling, 0, false, 0, 
		#ifndef DMS_CROSS_PLATFORMS
		NEXUS_VideoDecoderDecodeMode_eAll
		#endif
	};

    double timeSeekRangeBegin;
    double timeSeekRangeEnd;
    NEXUS_PlaybackPosition playPosition;
    BSTD_UNUSED(currentState);

    if (!playback_ip->useNexusPlaypump) {
        /* resuming from Paused or Trickmode state: we dont need to worry about re-opening the HTTP session to the server in case pause/trickmode had disconnected the socket */
        /* this is because HTTP playback module will reconnect to the server & ask for correct byte offset when Nexus playback will call it for data and it is missing in HTTP cache */
        /* (only happens when IP data is pulled by Nexus Playback) */
        /* TODO: not sure what state was being set here in the old code, may need to experiment w/ different format playback when taking it out */
        playback_ip->playback_state = B_PlaybackIpState_ePlaying;
        BDBG_MSG(("%s: Calling Nexus Play", __FUNCTION__));
        if (NEXUS_Playback_Play(playback_ip->nexusHandles.playback) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: Failed to Resume Nexus Playback", __FUNCTION__));
            rc = B_ERROR_UNKNOWN;
            goto error;
        }
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            print_av_pipeline_buffering_status(playback_ip);
#endif
    }
    else {
        /* using Nexus Playpump, so this session must be using server side trickmodes (timeSeek and/or playspeed) */
        if (playback_ip->useSeekPosition == true) {
            /* seekPosition is already set, use this position (it is the new seek point) for normal play */
            playPosition = playback_ip->seekPosition;
        }
        else {
            /* seekPosition is not set */
            /* we are resuming from either trickplay or paused state, so note the current position, so that we can resume normal play from that point */
            if (B_PlaybackIp_HttpGetCurrentPlaybackPosition(playback_ip, &playPosition) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Failed to determine the current playback position", __FUNCTION__));
                goto error;
            }
            BDBG_MSG(("%s: resuming position %0.3f", __FUNCTION__, playPosition/1000.));
        }

        if (playback_ip->reOpenSocket == true || playback_ip->ipTrickModeSettings.method == B_PlaybackIpTrickModesMethod_UseRvuSpec)
        {
            /* reconnect to server for both trickmode case (other than when rate is 2) & pause w/ disconnect case */
            timeSeekRangeBegin = playPosition/1000.;
            if (playback_ip->psi.psiValid && playback_ip->psi.duration > 0)
                timeSeekRangeEnd = playback_ip->psi.duration/1000.;
            else
                timeSeekRangeEnd = 0.;
            /* build & send the new HTTP Get request to server so that it will send stream at new speed & time position */
            if ((rc = http_do_server_trick_modes_socket_transition(playback_ip, timeSeekRangeBegin, timeSeekRangeEnd, 1 /* play speed */)) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Failed to update server about the trick play transition\n", __FUNCTION__));
                goto error;
            }
            playback_ip->reOpenSocket = false;

            /* flush the AV decoders & Nexus playpump buffers so that we cleanly start decode from the new stream position */
            if (B_PlaybackIp_UtilsFlushAvPipeline(playback_ip)) {
                BDBG_ERR(("%s: ERROR: Failed to flush the AV pipeline\n", __FUNCTION__));
                rc = B_ERROR_UNKNOWN;
                goto error;
            }
            /* store last position we seek/resume from as this will be used in currentPosition calculation when we receive 1st PTS from this new stream point */
            playback_ip->lastSeekPosition = playPosition;
            playback_ip->lastSeekPositionSet = true;
        }

        /* now tell decoder & playpump to resume */
        ipTrickModeSettings.rate = 1; /* normal speed */
        ipTrickModeSettings.frameRepeat = 1; /* normal speed */
        ipTrickModeSettings.seekBackward = 0; /* normal play */
        ipTrickModeSettings.method = playback_ip->ipTrickModeSettings.method;
        if ((rc = updateNexusPlaypumpDecodersState(playback_ip, &ipTrickModeSettings)) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during trickmode transition\n", __FUNCTION__));
            goto error;
        }
		playback_ip->ipTrickModeSettings = ipTrickModeSettings;
    }
    BDBG_MSG(("%s: Successfully Resumed IP Playback, ip state %d, fd %d", __FUNCTION__, playback_ip->playback_state, playback_ip->socketState.fd));
    rc = B_ERROR_SUCCESS;
error:
    return rc;
}

/*
Summary:
Play/Resume playback.
 */
B_PlaybackIpError 
B_PlaybackIp_Play(
    B_PlaybackIpHandle playback_ip
    ) 
{
    B_PlaybackIpError rc = B_ERROR_UNKNOWN;
    B_PlaybackIpState currentState;

    if (!playback_ip) {
        BDBG_ERR(("%s: ERROR: incorrect params: playback_ip %p\n", __FUNCTION__, playback_ip));
        return B_ERROR_INVALID_PARAMETER;
    }

    BDBG_MSG(("%s: Resuming plaback_ip %p, current state %d, pause method %d, fd %d", __FUNCTION__, playback_ip, playback_ip->playback_state, playback_ip->ipTrickModeSettings.pauseMethod, playback_ip->socketState.fd));

    if (playback_ip->playback_state != B_PlaybackIpState_ePaused && 
            playback_ip->playback_state != B_PlaybackIpState_ePlaying && 
            playback_ip->playback_state != B_PlaybackIpState_eTrickMode) {
        BDBG_ERR(("%s: ERROR: Can't do Play command in this state %d\n", __FUNCTION__, playback_ip->playback_state));
        return B_ERROR_INVALID_PARAMETER;
    }

    if (playback_ip->playback_state == B_PlaybackIpState_ePlaying)
        return B_ERROR_SUCCESS;

    lock_ip_session(playback_ip);
    currentState = playback_ip->playback_state;

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    if (playback_ip->hlsSessionEnabled) 
        rc = B_PlaybackIp_PlayHls(playback_ip);
    else
#endif
    if (playback_ip->protocol == B_PlaybackIpProtocol_eHttp)
        rc = B_PlaybackIp_PlayHttp(playback_ip, currentState);
#ifdef LIVEMEDIA_SUPPORT
    else if (playback_ip->protocol == B_PlaybackIpProtocol_eRtsp)
        rc = B_PlaybackIp_PlayRtsp(playback_ip);
#endif
    else {
        BDBG_ERR(("%s: ERROR: Play not supported for IP protocol %d", __FUNCTION__, playback_ip->protocol));
        rc = B_ERROR_NOT_SUPPORTED;
    }
    /* reset the paused position */
    playback_ip->seekPosition = 0;
    playback_ip->useSeekPosition = false;

    if (rc == B_ERROR_SUCCESS) {
        playback_ip->playback_state = B_PlaybackIpState_ePlaying;
        playback_ip->ipTrickModeSettings.rate = 1;
    }
    else {
        playback_ip->playback_state = currentState;
    }
    BDBG_MSG(("%s: Resuming to Play state, ip state %d", __FUNCTION__, playback_ip->playback_state));
    unlock_ip_session(playback_ip);
    return rc;
}

B_PlaybackIpError 
B_PlaybackIp_SeekRvu(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    BSTD_UNUSED(playback_ip);
    BSTD_UNUSED(ipTrickModeSettings);
    BDBG_ERR(("%s: ERROR: Rvu Seek is not yet supported", __FUNCTION__));
    return B_ERROR_SUCCESS;
}

B_PlaybackIpError 
B_PlaybackIp_SeekRtsp(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    BSTD_UNUSED(playback_ip);
    BSTD_UNUSED(ipTrickModeSettings);
    BDBG_ERR(("%s: ERROR: RTSP Seek is not yet supported", __FUNCTION__));
    return B_ERROR_SUCCESS;
}

B_PlaybackIpError 
B_PlaybackIp_SeekHttp(
    B_PlaybackIpHandle playback_ip,
    NEXUS_PlaybackPosition seekPosition, 
    B_PlaybackIpState currentState,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    B_PlaybackIpError rc = B_ERROR_SUCCESS;
    NEXUS_Error nrc;

    if (!playback_ip->useNexusPlaypump) {
        if (playback_ip->psi.duration && seekPosition >= playback_ip->psi.duration) {
            BDBG_WRN(("%s: asked position (relative flag %d, seek backward %d, pos relative %u, absolute %d) is outside the file duration %u", 
                        __FUNCTION__, ipTrickModeSettings->seekPositionIsRelative, ipTrickModeSettings->seekBackward, ipTrickModeSettings->seekPosition, seekPosition, playback_ip->psi.duration));
            seekPosition = 0;
        }
        BDBG_MSG(("%s: Calling Nexus Seek\n", __FUNCTION__));
        /* we need to unlock here, this is because for some streams that dont have index data, Nexus Seek */
        /* will invoke data callback of Http module. If we didn't release the lock here, the data callback wont */
        /* run as this lock serializes either this thread or data callback thread and thus seek would fail */

        unlock_ip_session(playback_ip);
        if ( (nrc = NEXUS_Playback_Seek(playback_ip->nexusHandles.playback, seekPosition)) != NEXUS_SUCCESS) {
            BDBG_ERR(("%s: ERROR: Failed to seek using NEXUS_Playback_Seek, rc %d", __FUNCTION__, nrc));
            rc = B_ERROR_UNKNOWN;
            goto error;
        }
        lock_ip_session(playback_ip);
        rc = B_ERROR_SUCCESS;
    }
    else {
        /* using Nexus Playpump to feed data, so server must support time seek */
        if (currentState == B_PlaybackIpState_ePaused) {
            double timeSeekRangeBegin;
            double timeSeekRangeEnd;
            /* logic for seeking during paused state */
            timeSeekRangeBegin = seekPosition/1000.;
            if (playback_ip->psi.psiValid && playback_ip->psi.duration > 0)
                timeSeekRangeEnd = playback_ip->psi.duration/1000.;
            else
                timeSeekRangeEnd = 0.;
            /* build & send the new HTTP Get request to server so that it will send data from new time position */
            if ((rc = http_do_server_trick_modes_socket_transition(playback_ip, timeSeekRangeBegin, timeSeekRangeEnd, 1 /* play speed */)) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Failed to update server about the trick play transition\n", __FUNCTION__));
                goto error;
            }
            /* clear the seek position, so that next play will start from the already seeked point */
            playback_ip->seekPosition = 0;

            /* flush the current pipeline so frame advance will show new frame */
            if (B_PlaybackIp_UtilsFlushAvPipeline(playback_ip)) {
                BDBG_ERR(("%s: ERROR: Failed to flush the AV pipeline\n", __FUNCTION__));
                rc = B_ERROR_UNKNOWN;
                goto error;
            }

            /* since we are in paused state, we need to resume playpump so that new data can be feed to the decoders */
            if (NEXUS_Playpump_SetPause(playback_ip->nexusHandles.playpump, false) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Playpump_SetPause() failed to enable pause", __FUNCTION__));
                rc = B_ERROR_UNKNOWN;
                goto error;
            }
            /* allow Ip_PlayPump thread to feed some more data from the socket */
            playback_ip->playback_state = B_PlaybackIpState_ePlaying;
            unlock_ip_session(playback_ip);
            /* TODO: this sleep seems too long, look into this */
            BKNI_Sleep(1000);
            /* data for 1 frame worth should be fed to decoder by now, so get the control back from Feeder thread */
            lock_ip_session(playback_ip);
            playback_ip->playback_state = B_PlaybackIpState_ePaused;

            if (playback_ip->nexusHandles.videoDecoder) {
                /* advance one frame to show the new seeked position */
                if (NEXUS_VideoDecoder_FrameAdvance(playback_ip->nexusHandles.videoDecoder) != NEXUS_SUCCESS) {
                    BDBG_ERR(("%s: ERROR: NEXUS_VideoDecoder_FrameAdvance() Failed \n", __FUNCTION__));
                    rc = B_ERROR_UNKNOWN;
                    goto error;
                }
                BDBG_MSG(("%s: Displayed the frame from the seeked position using NEXUS_VideoDecoder_FrameAdvance()", __FUNCTION__));
            }
            /* re-pause the playpump so that it doesn't expect or feed anymore data */
            if (NEXUS_Playpump_SetPause(playback_ip->nexusHandles.playpump, true) != NEXUS_SUCCESS) {
                BDBG_ERR(("%s: NEXUS_Playpump_SetPause() failed to disable pause", __FUNCTION__));
                rc = B_ERROR_UNKNOWN;
                goto error;
            }

            /* close the current socket */
            if (playback_ip->netIo.shutdown && !playback_ip->serverClosed)
                playback_ip->netIo.shutdown(playback_ip->securityHandle, 0);
            /* set flag to reconnect at the pause time */
            close(playback_ip->socketState.fd);
            playback_ip->reOpenSocket = true;
            BDBG_MSG(("%s: Completed Seek During Pause", __FUNCTION__));
        }
        rc = B_ERROR_SUCCESS;
    }
error:
    return rc;
}

B_PlaybackIpError 
B_PlaybackIp_Seek(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    )
{
    B_PlaybackIpError rc = B_ERROR_UNKNOWN;
    NEXUS_PlaybackPosition seekPosition; /* absolute position in msec */
    B_PlaybackIpState currentState;

    if (!playback_ip || !ipTrickModeSettings) {
        BDBG_ERR(("%s: ERROR: incorrect params: playback_ip %p, trickMode %p\n", __FUNCTION__, playback_ip, ipTrickModeSettings));
        return B_ERROR_INVALID_PARAMETER;
    }

    if (playback_ip->psi.liveChannel) {
        /* TODO: take this check out once trickmodes on live channels are supported */
        BDBG_WRN(("%s: ERROR: Seek not supported for LIVE Channels yet, ignoring seek command", __FUNCTION__));
        return B_ERROR_SUCCESS;
    }

    if (playback_ip->protocol != B_PlaybackIpProtocol_eHttp) {
        /* TODO: take this check out once RTSP & other protocols are supported */
        BDBG_ERR(("%s: ERROR: Seek not supported for IP protocol %d, ignoring seek command", __FUNCTION__, playback_ip->protocol));
        return B_ERROR_SUCCESS;
    }

    currentState = playback_ip->playback_state;
    BDBG_MSG(("%s: Seeking to %u msec of %s position in %s direction, fd %d, ip state %d\n", __FUNCTION__, ipTrickModeSettings->seekPosition, 
                ipTrickModeSettings->seekPositionIsRelative? "relative":"absolute", 
                ipTrickModeSettings->seekBackward? "backward":"forward", playback_ip->socketState.fd, currentState)); 
    if (playback_ip->playback_state == B_PlaybackIpState_ePlaying) {
        /* Pause the playback only if we are currently in the playing state */
        /* we are forcing to use the DisconnectAndSeek method of pausing as seek will anyway require reconnecting at a different offset */
        ipTrickModeSettings->pauseMethod = B_PlaybackIpPauseMethod_UseDisconnectAndSeek;
        if (B_PlaybackIp_Pause(playback_ip, ipTrickModeSettings)) {
            BDBG_ERR(("%s: ERROR: Failed to pause Ip playback during Seek operation", __FUNCTION__));
            goto error;
        }
    }

    lock_ip_session(playback_ip);
    if (playback_ip->serverClosed) {
        /* reset this flag incase server was done sending the complete file as we may be seeking back */
        /* if server had not closed, Ip_Pause() will set the reOpenSocket flag */
        playback_ip->serverClosed = false;
        /* since server has closed, set a flag to re-open the socket */
        playback_ip->reOpenSocket = true; /* this flags tells HTTP layer to reopen the connection */
    }

    /* determine absolute seek position if user specified relative position */
    if (ipTrickModeSettings->seekPositionIsRelative) {
        /* get the current position */
        NEXUS_PlaybackPosition currentPosition;
        if (B_PlaybackIp_HttpGetCurrentPlaybackPosition(playback_ip, &currentPosition) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: Failed to determine the current playback position\n", __FUNCTION__));
            goto error;
        }
        BDBG_MSG(("%s: current position %0.3f", __FUNCTION__, currentPosition/1000.));

        /* adjust relative value into the current position determined by the Pause() call above */
        if (ipTrickModeSettings->seekBackward)
            seekPosition = currentPosition - ipTrickModeSettings->seekPosition; 
        else
            seekPosition = currentPosition + ipTrickModeSettings->seekPosition; 
    }
    else {
        seekPosition = ipTrickModeSettings->seekPosition; 
    }
    BDBG_MSG(("%s: updated seek position from %0.3f to %0.3f (in sec) \n", __FUNCTION__, ipTrickModeSettings->seekPosition/1000., seekPosition/1000.));

    /* Now seek to the new position */
#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    if (playback_ip->hlsSessionEnabled) 
        rc = B_PlaybackIp_SeekHls(playback_ip, seekPosition);
    else
#endif
    if (playback_ip->protocol == B_PlaybackIpProtocol_eHttp)
        rc = B_PlaybackIp_SeekHttp(playback_ip, seekPosition, currentState, ipTrickModeSettings);
    else if (playback_ip->protocol == B_PlaybackIpProtocol_eRtsp)
        rc = B_PlaybackIp_SeekRtsp(playback_ip, ipTrickModeSettings);
    else if (playback_ip->ipTrickModeSettings.method == B_PlaybackIpTrickModesMethod_UseRvuSpec)
        rc = B_PlaybackIp_SeekRvu(playback_ip, ipTrickModeSettings);
    else {
        BDBG_ERR(("%s: ERROR: Seek not supported for IP protocol %d", __FUNCTION__, playback_ip->protocol));
        rc = B_ERROR_NOT_SUPPORTED;
    }

    if (rc == B_ERROR_SUCCESS) {
        /* store the position to seek to, it will be used in the Ip_Play() */
        playback_ip->seekPosition = seekPosition;
        /* store last position we seek/resume from as this will be used in currentPosition calculation when we receive 1st PTS from this new stream point */
        playback_ip->lastSeekPosition = seekPosition;
        playback_ip->lastSeekPositionSet = true;
        playback_ip->useSeekPosition = true;
    }
    unlock_ip_session(playback_ip);

    if (currentState == B_PlaybackIpState_ePlaying) {
        /* we only resume to play state if we were originally in the playing state */
        if (B_PlaybackIp_Play(playback_ip)) {
            BDBG_ERR(("%s: ERROR: Failed to play\n", __FUNCTION__));
            rc = B_ERROR_UNKNOWN;
            return rc;
        }
    }
    rc = B_ERROR_SUCCESS;
    BDBG_MSG(("%s: Seeked to position %u msec, fd %d\n", __FUNCTION__,  seekPosition, playback_ip->socketState.fd));
    return rc;
error:
    unlock_ip_session(playback_ip);
    return rc;
}

B_PlaybackIpError B_PlaybackIp_GetTrickModeByRate(
    B_PlaybackIpHandle playback_ip,
    int rate,
    B_PlaybackIpTrickModesSettings *trickMode
    ) 
{
    /* bool allow_brcm_trick_modes = bsettop_get_config("no_brcm_trick_modes") == NULL; */
    BSTD_UNUSED(playback_ip);
    BSTD_UNUSED(trickMode);
    BDBG_MSG(("%s: rate: %d", __FUNCTION__, rate));
    
    /* this function is supposed to return the trickmode parameters for a given rate. However, it doesn't make sense for IP trickmodes */
    /* as IP trickmodes parameters are quite high level at this time (e.g. take only rate and trick mode type as input) */
    return B_ERROR_SUCCESS;
}

B_PlaybackIpError 
B_PlaybackIp_TrickModeHttp(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings,
    B_PlaybackIpState currentState
    ) 
{
    B_PlaybackIpError rc = B_ERROR_UNKNOWN;
    double timeSeekRangeBegin;
    double timeSeekRangeEnd;
    NEXUS_PlaybackPosition currentPosition;

    if (!playback_ip->useNexusPlaypump) {
        if ((rc = http_do_client_side_trick_modes_using_playback(playback_ip, ipTrickModeSettings)) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: Trick mode transition failed \n", __FUNCTION__));
            goto error;
        }
#ifdef BDBG_DEBUG_BUILD
        if (playback_ip->ipVerboseLog)
            print_av_pipeline_buffering_status(playback_ip);
#endif
    }
    else {
        bool slowMotion;
        if (ipTrickModeSettings->absoluteRateDefined) {
            slowMotion = (-NEXUS_NORMAL_PLAY_SPEED < ipTrickModeSettings->absoluteRate && ipTrickModeSettings->absoluteRate < NEXUS_NORMAL_PLAY_SPEED);
        }
        else {
            slowMotion = false;
        }
        if (slowMotion) {
            /* for slow motion trickmodes, we manipulate stc */
            playback_ip->stcTrickMode = true;
            playback_ip->stcRate = ipTrickModeSettings->absoluteRate;
        }
        else if (ipTrickModeSettings->rate == 2) {
            /* for 2x, we run stc at 2x the speed */
            playback_ip->stcTrickMode = true;
            playback_ip->stcRate = 2*NEXUS_NORMAL_PLAY_SPEED;
        }
        else {
            playback_ip->stcTrickMode = false;
            playback_ip->stcRate = NEXUS_NORMAL_PLAY_SPEED;
        }

        if (!playback_ip->stcTrickMode) {
            /* using Nexus Playpump, so this session must be using server side trickmodes (playspeed) */
            /* we are resuming from the trickplay state, so note the current position, so that we can resume from that point */
            if (B_PlaybackIp_HttpGetCurrentPlaybackPosition(playback_ip, &currentPosition) != B_ERROR_SUCCESS) {
                BDBG_WRN(("%s: Failed to determine the current playback position", __FUNCTION__));
                goto error;
            }
            BDBG_MSG(("%s: new trickplay starting position %0.3f", __FUNCTION__, currentPosition/1000.));

            /* build & send the new HTTP Get request to server so that it will send stream at new speed & time position */
            timeSeekRangeBegin = currentPosition/1000.;
            if (playback_ip->psi.psiValid && playback_ip->psi.duration > 0)
                timeSeekRangeEnd = playback_ip->psi.duration/1000.;
            else if (ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UseRvuSpec || ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UseTimeSeekRange) {
#define FRAMES_DISPLAYED_PER_SEC 5.0
                if (ipTrickModeSettings->rate > 0) {
                    playback_ip->timeSeekRangeInterval = ipTrickModeSettings->rate*1.0 / FRAMES_DISPLAYED_PER_SEC;
                    timeSeekRangeEnd = timeSeekRangeBegin + playback_ip->timeSeekRangeInterval;
                    playback_ip->initialTimeSeekRangeBegin = timeSeekRangeEnd; 
                }
                else {
                    playback_ip->timeSeekRangeInterval = ipTrickModeSettings->rate*-1.0 / FRAMES_DISPLAYED_PER_SEC;
                    timeSeekRangeEnd = timeSeekRangeBegin;
                    timeSeekRangeBegin -= playback_ip->timeSeekRangeInterval;
                    playback_ip->initialTimeSeekRangeBegin = timeSeekRangeBegin; 
                }
            }
            else
                timeSeekRangeEnd = 0.;
            BDBG_MSG(("%s: timeSeekRangeBegin %0.3f, timeSeekRangeEnd %0.3f, interval %0.3f, state %d", 
                        __FUNCTION__, timeSeekRangeBegin, timeSeekRangeEnd, playback_ip->timeSeekRangeInterval, playback_ip->playback_state));
            if ((rc = http_do_server_trick_modes_socket_transition(playback_ip, timeSeekRangeBegin, timeSeekRangeEnd, ipTrickModeSettings->rate /* play speed */)) != B_ERROR_SUCCESS) {
                BDBG_ERR(("%s: Failed to update server about the trick play transition\n", __FUNCTION__));
                goto error;
            }
            /* store last position we start trickmode from as this will be used in currentPosition calculation when we receive 1st PTS from this new stream point */
            playback_ip->lastSeekPosition = currentPosition;
            playback_ip->lastSeekPositionSet = true;

            /* flush the AV decoders & Nexus playpump buffers so that we cleanly start decode from the new stream position */
#if 0
            if (currentState != B_PlaybackIpState_eTrickMode) 
#else		
			BSTD_UNUSED(currentState);
#endif
            {
                /* we dont flush pipeline if we are already in the trick mode state */
                if (B_PlaybackIp_UtilsFlushAvPipeline(playback_ip)) {
                    BDBG_ERR(("%s: ERROR: Failed to flush the AV pipeline\n", __FUNCTION__));
                    rc = B_ERROR_UNKNOWN;
                    goto error;
                }
            }
            /* set a flag to indicate to Ip_Play() to reopen the connection with normal speed when play is resumed */
            playback_ip->reOpenSocket = true;
        }
        else {
            /* we use STC trickplay for 0 < speeds < NORMAL */
            /* TODO: add this function */
        }

        /* now tell decoder & playpump to resume */
        if ((rc = updateNexusPlaypumpDecodersState(playback_ip, ipTrickModeSettings)) != B_ERROR_SUCCESS) {
            BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during trickmode transition\n", __FUNCTION__));
            goto error;
        } 
        /* reset flag indicating stc trickmode */
        playback_ip->stcTrickMode = false;
        playback_ip->stcRate = NEXUS_NORMAL_PLAY_SPEED;
    }
    return B_ERROR_SUCCESS;
error:
    return rc;
}

#ifdef LIVEMEDIA_SUPPORT
B_PlaybackIpError 
B_PlaybackIp_TrickModeRtsp(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    ) 
{
    B_PlaybackIp_RtspCtrlCmd cmd;

    /* send command to server to start trick play at given rate */
    cmd.type = B_PlaybackIp_RtspCtrlCmdType_Resume;
    /* (Note: start=-1 means 'resume'; end=-1 means 'play to end') */
    cmd.params.start = -1;
    cmd.params.end = -1;
    cmd.params.scale = ipTrickModeSettings->rate;
    B_PlaybackIp_liveMediaSessionRTSPControl(playback_ip->lm_context, &cmd);

    /* disable the live mode so that feeder thread doesn't detect any discontinuities introduced due to longer n/w latency */
    playback_ip->disableLiveMode = true;

    /* update decoder settings */
    ipTrickModeSettings->frameRepeat = 0;
    if (ipTrickModeSettings->decodeMode > NEXUS_VideoDecoderDecodeMode_eI) {
        /* user specified invalid decodeMode, force to decode all pictures */
        ipTrickModeSettings->decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
    }
    if (updateNexusPlaypumpDecodersState(playback_ip, ipTrickModeSettings) != B_ERROR_SUCCESS) {
        BDBG_ERR(("%s: ERROR: failed to update nexus av decoder state during trickmode transition\n", __FUNCTION__));
        return B_ERROR_UNKNOWN;
    }
    return B_ERROR_SUCCESS;
}
#endif /* LIVEMEDIA_SUPPORT */


B_PlaybackIpError B_PlaybackIp_TrickMode(
    B_PlaybackIpHandle playback_ip,
    B_PlaybackIpTrickModesSettings *ipTrickModeSettings
    ) 
{
    B_PlaybackIpError rc = B_ERROR_UNKNOWN;
    B_PlaybackIpState currentState;

    if (!playback_ip || !ipTrickModeSettings) {
        BDBG_ERR(("%s: NULL params (playback_ip %p, trickModeSettings %p)\n", __FUNCTION__, playback_ip, ipTrickModeSettings));
        return B_ERROR_INVALID_PARAMETER;
    }

    if (playback_ip->psi.liveChannel) {
        /* TODO: take this check out once trickmodes on live channels are supported */
        BDBG_WRN(("%s: ERROR: Seek not supported for LIVE Channels yet, ignoring seek command", __FUNCTION__));
        return B_ERROR_SUCCESS;
    }

#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    if (playback_ip->hlsSessionEnabled) 
        /* for now, ignore trickmode commands for HLS sessions until as add this support */
        return B_ERROR_SUCCESS;
#endif
    if ((ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UseTimeSeekRange || 
         ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UsePlaySpeed || 
         ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UseRvuSpec) && !playback_ip->useNexusPlaypump) {
        BDBG_ERR(("%s: trick mode method %d requires App to configure IP Session using Nexus Plapump, ignoring the command", __FUNCTION__, ipTrickModeSettings->method));
        return B_ERROR_SUCCESS;
    }
    if (ipTrickModeSettings->method == B_PlaybackIpTrickModesMethod_UseByteRange && playback_ip->useNexusPlaypump) {
        BDBG_ERR(("%s: trick mode method %d requires App to configure IP Session using Nexus Playback, ignoring the command", __FUNCTION__, ipTrickModeSettings->method));
        return B_ERROR_SUCCESS;
    }
    if (ipTrickModeSettings->absoluteRateDefined && -NEXUS_NORMAL_PLAY_SPEED < ipTrickModeSettings->absoluteRate && ipTrickModeSettings->absoluteRate < 0) {
        BDBG_ERR(("%s: slow rewind rate %d is not yet supported", __FUNCTION__, ipTrickModeSettings->absoluteRate));
        return B_ERROR_SUCCESS;
    }
    BDBG_MSG(("%s: trick mode method %d, rate %d\n", __FUNCTION__, ipTrickModeSettings->method, ipTrickModeSettings->rate));
    
    lock_ip_session(playback_ip);

    /* change the state to EnteringTrickMode until we finish the trickmode work */
    currentState = playback_ip->playback_state;
    playback_ip->playback_state = B_PlaybackIpState_eEnteringTrickMode;

    if (playback_ip->protocol == B_PlaybackIpProtocol_eHttp)
        rc = B_PlaybackIp_TrickModeHttp(playback_ip, ipTrickModeSettings, currentState);
#ifdef LIVEMEDIA_SUPPORT
    else if (playback_ip->protocol == B_PlaybackIpProtocol_eRtsp)
        rc = B_PlaybackIp_TrickModeRtsp(playback_ip, ipTrickModeSettings);
#endif
#if 0
#ifdef B_HAS_HLS_PROTOCOL_SUPPORT
    else if (playback_ip->hlsSessionEnabled)
        rc = B_TrickModebackIp_TrickModeHls(playback_ip);
#endif
    else if (playback_ip->ipTrickModeSettings.method == B_TrickModebackIpTrickModesMethod_UseRvuSpec)
        rc = B_TrickModebackIp_TrickModeRvu(playback_ip);
#endif
    else {
        BDBG_ERR(("%s: ERROR: TrickMode not supported for IP protocol %d", __FUNCTION__, playback_ip->protocol));
        rc = B_ERROR_NOT_SUPPORTED;
    }

    if (rc == B_ERROR_SUCCESS) {
        playback_ip->playback_state = B_PlaybackIpState_eTrickMode;
        playback_ip->ipTrickModeSettings = *ipTrickModeSettings;
    }
    else {
        playback_ip->playback_state = currentState;
    }
    unlock_ip_session(playback_ip);
    return rc;

}

#endif /* LINUX || VxWorks */
