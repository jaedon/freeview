/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "PumpingThread.h"

#include <string>

#include <nrdbase/Log.h>
#include <nrdbase/PerformanceMarker.h>
#include <nrdbase/ScopedMutex.h>

#include <nrdnet/AseTimeVal.h>

#include "AdaptiveStreamingPlayer.h"
#include "IPlaybackReporter.h"
#include "PlayerState.h"

using namespace netflix;
using namespace netflix::ase;
using namespace netflix::device;
using namespace netflix::mediacontrol;

static const uint32_t MAX_BUFFERING_ERROR_WAIT = 50;
static const uint32_t MAX_NUM_DEVICE_NOT_READY_ERROR_WAIT = 1000;
static const uint32_t STATUS_REPORT_INTERVAL = 2000;
static const uint32_t SLEEP_WAITING_READY_FOR_PLAY = 30; // 30 milisec when used with usleep
static const Time WAIT_WHILE_FEEDING(30);
static const Time WAIT_WHILE_IDLING(500);

//////////
//
// LocalState
//
//////////
class PumpingThread::LocalState
{
public:
    LocalState() :
        m_currSpeed(IPlaybackDevice::PAUSED_SPEED),
        m_numSuccessivePendingReturnsFromAudioFeedData(0),
        m_numSuccessivePendingReturnsFromVideoFeedData(0)
    {
        resetCounters();
    }

    void resetCounters()
    {
        m_deliveredAudioBlocks = 0;
        m_deliveredVideoBlocks = 0;
        m_attemptedAudioBlocks = 0;
        m_attemptedVideoBlocks = 0;
        m_logOnce = true;

        m_firstAudioGetBlockOutcomeLogged = false;
        m_firstValidAudioGetBlockOutcomeLogged = false;
        m_firstVideoGetBlockOutcomeLogged = false;
        m_firstValidVideoGetBlockOutcomeLogged = false;

        m_bufferingErrorWaitCount = 0;

        m_deviceNotReadyForPlaybackStartWaitCount = 0;

        m_lastFeedBlockReportTime = AseTimeVal::ZERO;
        m_lastGetBlockReportTime = AseTimeVal::ZERO;

        m_ptsForLastDeliveredBlock = INVALID_PTS;
    }

public:
    void deliveredBlock(IPlaybackDevice::StreamType streamType)
    {
        if (streamType == IPlaybackDevice::AUDIO)
        {
            ++m_deliveredAudioBlocks;
            m_numSuccessivePendingReturnsFromAudioFeedData = 0;
        }
        else
        {
            ++m_deliveredVideoBlocks;
            m_numSuccessivePendingReturnsFromVideoFeedData = 0;
        }
    }

    void pendingBlock(IPlaybackDevice::StreamType streamType)
    {
        if (streamType == IPlaybackDevice::AUDIO)
            ++m_numSuccessivePendingReturnsFromAudioFeedData;
        else
            ++m_numSuccessivePendingReturnsFromVideoFeedData;
    }

    bool checkBlocksDelivered(uint32_t currentPts, uint32_t minimumDataInDevice)
    {

        if ( (m_deliveredVideoBlocks > 0) && (m_deliveredAudioBlocks > 0) )
        {
            return true;
        }
        else if ( (currentPts != INVALID_PTS) && (m_ptsForLastDeliveredBlock != INVALID_PTS) )
        {
            if ( m_ptsForLastDeliveredBlock > currentPts + minimumDataInDevice )
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    }

    bool checkDeviceNotReadyForPlaybackStart(uint32_t targetSpeed)
    {
#ifndef BUILD_DEBUG
        NRDP_UNUSED(targetSpeed);
#endif
        ++m_deviceNotReadyForPlaybackStartWaitCount;

        // Don't log this a thousand times before we time out after 30 sec.
        // About every half-second is enough.
        if((m_deviceNotReadyForPlaybackStartWaitCount % 16 == 0))
        {
            NTRACE(TRACE_MEDIACONTROL, "playbackDevice_->setSpeed: %u fails."
                   "because device not ready and feedData returns pending."
                   " Waited %d times", targetSpeed,
                   m_deviceNotReadyForPlaybackStartWaitCount);
        }

        bool result = (m_deviceNotReadyForPlaybackStartWaitCount >
                       MAX_NUM_DEVICE_NOT_READY_ERROR_WAIT);

        return result;
    }

    bool checkBufferingErrorWaitCount()
    {
        ++m_bufferingErrorWaitCount;

        return (m_bufferingErrorWaitCount > MAX_BUFFERING_ERROR_WAIT);
    }

    bool checkUnderflowAttempts()
    {
        if (m_logOnce && ((m_attemptedVideoBlocks == 0) || (m_attemptedAudioBlocks == 0)))
        {
            m_logOnce = false;
            return true;
        }

        return false;
    }

public:
    // The current speed of the playback device (it is paused right after opened).
    uint32_t m_currSpeed;

    // Number of delivered audio/video blocks since buffering starts.
    uint32_t m_deliveredAudioBlocks;
    uint32_t m_deliveredVideoBlocks;
    uint32_t m_attemptedAudioBlocks;
    uint32_t m_attemptedVideoBlocks;
    bool m_logOnce;

    // These allow the first getBlock outcomes to be logged after start or
    // rebuffer.
    bool m_firstAudioGetBlockOutcomeLogged;
    bool m_firstValidAudioGetBlockOutcomeLogged;
    bool m_firstVideoGetBlockOutcomeLogged;
    bool m_firstValidVideoGetBlockOutcomeLogged;


    // Number of waiting cycles before reporting buffering error.
    uint32_t m_bufferingErrorWaitCount;

    // Number of waiting cycles before reporting giving up on setting playback speed
    uint32_t m_deviceNotReadyForPlaybackStartWaitCount;

    AseTimeVal m_lastFeedBlockReportTime;
    AseTimeVal m_lastGetBlockReportTime;

    uint32_t   m_ptsForLastDeliveredBlock;

    // Audio/Video context for data pumping
    IAdaptiveStreamingPlayer::AvPumpingContext m_avPumpingContext;

    unsigned int m_numSuccessivePendingReturnsFromAudioFeedData;
    unsigned int m_numSuccessivePendingReturnsFromVideoFeedData;
};

//////////
//
// PumpingThread
//
//////////
PumpingThread::PumpingThread(AdaptiveStreamingPlayer &player,
                             shared_ptr<PlayerState> playerState,
                             shared_ptr<IPlaybackReporter> playbackReporter,
                             bool faststart)
    : McThread(MCTHREAD_MUTEX, &THREAD_MC_PUMPING),
      m_state(PUMPING_STOPPED),
      m_player(player),
      m_playerState(playerState),
      m_playbackReporter(playbackReporter),
      m_faststart(faststart),
      m_challengeGenerated(false),
      m_challengeProvided(false),
      m_loggedNoDrm(false),
      m_targetSpeed(0),
      m_slowPumpingEnterThreshold(0),
      m_slowPumpingExitThreshold(0),
      m_sleepDuringSlowPumping(1000)
{
}

//////////
//
// startDataPumping
//
//////////
void PumpingThread::startDataPumping()
{
    NTRACE(TRACE_MEDIACONTROL, "startDataPumping() %d", m_state);

    assert(m_state == PUMPING_STOPPED);

    lock();

    m_state = PUMPING;
    pinch();

    unlock();
}

//////////
//
// stopDataPumping
//
//////////
bool PumpingThread::stopDataPumping()
{
    static const Time WAIT_FOR_PUMPING_STOPPED(30);

    NTRACE(TRACE_MEDIACONTROL, "stopDataPumping() %d", m_state);

    // Change the state into STOP_PUMPING to request stopping in the pumping
    // thread, wait for the completion of stopping by periodically checking
    // whether the state has been changed into PUMPING_STOPPED.

    lock();

    // if we're not pumping, ignore
    if (m_state == PUMPING_STOPPED)
    {
        NTRACE(TRACE_MEDIACONTROL, "PumpingThread already stopped");
        unlock();
        return false;
    }

    m_state = STOP_PUMPING;

    pinch();

    while (m_state != PUMPING_STOPPED)
        wait(WAIT_FOR_PUMPING_STOPPED);

    unlock();

    return true;
}

//////////
//
// setPlaybackSpeed
//
//////////
void PumpingThread::setPlaybackSpeed(uint32_t speed)
{
    NTRACE(TRACE_MEDIACONTROL, "pumpingThread: set targetSpeed %d", speed);
    m_targetSpeed = speed;
}

//////////
//
// Run
//
//////////
void PumpingThread::Run()
{
    // lock the thread
    ScopedMutex lock(m_workerMutex);

    // Load media.
    if (!m_player.load())
    {
        Log::warn(TRACE_MEDIACONTROL, "pumpingThread: load failed\n");
        return;
    }

    // have the player notify any listeners of current tracks
    m_player.notifyTracksChanged();

    // Loading is successful, enter the pumping loop from here forward. The post
    // conditions of successful loading are:
    //
    //   1) a vector of media streams is created.
    //   2) the playback device is opened.
    //   3) the stream manager is created.

    m_player.notifyLoadingComplete(); // confirm loading-complete.

    // Local state variables
    LocalState localState;

    // Deal with the challenge now, if we can
    NFErr err = handleLicense();
        // We'll drop into the idle-wait-on-error case below if there's an error

    if ( err.ok() )
    {
        // Initialize the players after getting the challenge, in parallel with the challenge
        err = m_player.initializePlayers();
        m_playbackReporter->reportKeyStartplayEvent("playersinitialized");
    }

    m_player.obtainSlowPumpingParas(m_slowPumpingEnterThreshold, m_slowPumpingExitThreshold, m_sleepDuringSlowPumping);

    while (m_running)
    {
        PERFORMANCE_MARKER_SCOPED("pumping.loop");

        {
            PERFORMANCE_MARKER_SCOPED("pumping.loop.process");

            NFErr err;

            // Data pumping is stopped, the pumping thread just idles.
            if (m_state == PUMPING_STOPPED)
            {
                PERFORMANCE_MARKER_SCOPED("pumping.loop.stopped");

                wait(WAIT_WHILE_IDLING);
                continue;
            }

            if (m_state == STOP_PUMPING)
            {
                PERFORMANCE_MARKER_SCOPED("pumping.loop.stopping");

                // If stopping of data pumping is requested, flush and then change the
                // state to signal completion.
                stopPumping(localState);

                m_state = PUMPING_STOPPED;

                continue;
            }

            // If "error_" or "abort_" is set, the pumping thread just idles.
            if (m_playerState->m_error || m_player.abort_)
            {
                PERFORMANCE_MARKER_SCOPED("pumping.loop.error");

                wait(WAIT_WHILE_IDLING);
                continue;
            }

            // check if the license retriever failed for any reason
            if (m_player.licenseRetriever_.get())
            {
                err = m_player.licenseRetriever_->checkStatus();
                if (!err.ok())
                {
                    Log::info(TRACE_MEDIACONTROL,
                              "LicenseHandler returned %s stopping pumping",
                              err.toString().c_str());
                    m_playerState->m_error = true;
                    continue; // loop back for error
                }
            }

            err = handleLicense();
            if ( !err.ok() )
                continue;

            if (!m_playerState->m_licenseAcquired && !m_faststart)
            {
                PERFORMANCE_MARKER_SCOPED("pumping.loop.license");

                // if we're not faststart, wait for the license
                wait(WAIT_WHILE_IDLING);
                continue;
            }

            // If playback is not halted and "dataExhausted_" is set, kick off the
            // re-buffering procedure. However, if "endOfStream_" is already set, we
            // should ignore the data exhaustion.
            if (!(m_playerState->m_audioEndOfStream && m_playerState->m_videoEndOfStream)
                && !m_playerState->m_halted && m_player.dataExhausted_)
            {
                PERFORMANCE_MARKER_SCOPED("pumping.loop.rebuffer");

                NTRACE(TRACE_MEDIACONTROL, "PumpingThread detected rebuffer");
                m_player.rebuffer(localState.m_avPumpingContext, localState.m_currSpeed);

                // reset our local counters
                localState.resetCounters();
                continue;
            }

            // Check for any changes in playback speed
            if( NFErr_OK != checkForSpeedChange(localState)){
                // sleep here to give CPU to other thread.
                // speed change is from PAUSE to PLAY or from PLAY to PAUSE
                // when it is from PLAY to PAUSE, it will return almost immediately
                // when it is from PAUSE to PLAY, it may take time if underlying device
                // does not have enough data to display to prevent immediate rebuffering

                // while it was in the PAUSE state, pumping has been happening,
                // and we can think that enough data has been pumped already. so
                // when it is transitioning from PAUSE to PLAY, and if device is
                // not ready, we can safely sleep here little bit because most
                // of case, we don't need to feed blocks at full speed.

                wait(SLEEP_WAITING_READY_FOR_PLAY);
            }

            // Check for any changes to audio track
            if (checkForAudioTrackChange())
            {
                // audio track changed, reset avPumpingContext
                localState.m_avPumpingContext.audioReadPTS_ = 0;
                localState.m_avPumpingContext.currAudioBlock_.reset();
            }

            // pump data
            {
                if (pumpData(localState))
                {
                    // pipeline ok, loop back to check for state changes
                    if ( (localState.m_deliveredVideoBlocks > m_slowPumpingEnterThreshold) &&
                            (localState.m_deliveredVideoBlocks < m_slowPumpingExitThreshold) &&
                        (localState.m_deliveredVideoBlocks % 10 == 0) )
                    {
                        // The player does not need to wait till all data in the streaming buffer have been delivered to DPI
                        // to start the playback.
                        if ( m_playerState->m_halted )
                            checkEndOfBuffering(localState);

                        // sleep a short time during slow pumping period
                        usleep(m_sleepDuringSlowPumping);
                    }
                    continue;
                }
            }

            // make sure playback device was ready
            if (!checkPlaybackReady(localState))
            {
                // error was flagged, loopback for error state
                continue;
            }

            // When it gets to here, the possible conditions are:
            //   1) "endOfStream_" is set.
            //   2) stream manager is temporarily out-of-data.
            //   3) internal pipeline of playback device is full.
            // The pumping thread should sleep for a short time and continue.

            // check for end of buffering
            if (!checkEndOfBuffering(localState))
            {
                continue; // error state set, loop back for it
            }
        }

        // wait a bit and loop back
        wait(WAIT_WHILE_FEEDING);
    }

    // By design, data pumping must be stopped before the pumping thread is asked to
    // terminate. So when it gets to here, the playback device is already flushed,
    // the stream manager is already stopped, and all device memory buffers are freed.

    assert(m_state == PUMPING_STOPPED);

    // Clear the playback device license store on exit from data pumping if needed
    if (m_playerState->m_licenseNeedClear)
    {
        NTRACE(TRACE_MEDIACONTROL, "clearing license at end of pumping thread");
        m_player.clearLicense();
    }

    // Unload media.
    m_player.unload();
}

//////////
//
// stopPumping
//
//////////
void PumpingThread::stopPumping(LocalState &localState)
{
    // flush the player
    m_player.flush(localState.m_avPumpingContext, /*out*/localState.m_currSpeed);

    // reset our local state
    localState.resetCounters();

    // wake up anybody waiting for us to stop
    pinch();
}

//////////
//
// handleLicense
//
//////////
NFErr PumpingThread::handleLicense()
{
    // generate and provide the challenge to the license handler
    NFErr err;

    if (!m_playerState->m_licenseAcquired &&
        (!m_faststart ||
         (!m_playerState->m_drmHeaderParsed &&
          m_player.playbackDevice_->drmHeaderReceived())))
    {
        if (!m_playerState->m_drmHeaderParsed)
        {
            NTRACE(TRACE_MEDIACONTROL, "set drmHeaderParsed true");
            m_playerState->m_drmHeaderParsed = true;
        }

        // generate the challenge for drm
        err = generateChallenge();
        if (!err.ok())
        {
            if (err == NFErr_NotAllowed)
            {
                // no drm, assume challenge was provided
                m_challengeProvided = true;
            }
            else
            {
                return err;     // loop back to error state
            }
        }

        // provide the challenge to the LicenseHandler
        err = provideChallenge();
        if (!err.ok())
            return err; // loop back to error state
    }

    return err;
}

//////////
//
// generateChallenge
//
//////////
NFErr PumpingThread::generateChallenge()
{
    // if we already got the challenge, ignore
    if (m_challengeGenerated)
        return NFErr_OK;

    NTRACE(TRACE_MEDIACONTROL, "PumpingThread requesting challenge");
    NFErr err = m_player.generateChallenge();

    // NotAllowed means no drm and we allow that through
    if (!err.ok() && !err.contains(NFErr_NotAllowed))
    {
        m_playerState->m_error = true;

        err.push(NFErr_DRMFailed);
        err.push(NFErr_MC_OpenDeviceFailure);

        m_player.reportError(err);

        return err;
    }

    m_challengeGenerated = true;
    m_playbackReporter->reportKeyStartplayEvent("challengegenerated");

    return err;
}

//////////
//
// provideChallenge
//
//////////
NFErr PumpingThread::provideChallenge()
{
    // if we already requested the license, ignore
    if (m_challengeProvided)
        return NFErr_OK;

    NFErr err = m_player.acquireLicense();

    if (!err.ok())
    {
        // mark the error
        m_playerState->m_error = true;

        // push a MC error to top of stack and report it
        err.push(NFErr_MC_AcquireLicenseFailure);
        m_player.reportError(err);

        // send log info
        Log::info(TRACE_MEDIACONTROL,
                  "PumpingThread::provideChallenge() failed : %s",
                  err.toString().c_str());

        return err;
    }

    m_challengeProvided = true;

    return err;
}

//////////
//
// checkForSpeedChange
//
//////////
NFErr PumpingThread::checkForSpeedChange(LocalState &localState)
{
    // If the requested target speed is different from the currently applied
    // speed, update the playback device with the new target speed and remember
    // it as the new current speed.
    if (m_targetSpeed == localState.m_currSpeed)
        return NFErr_OK; // nothing to do

    // copy m_targetSpeed to local in case player changes it
    uint32_t speed = m_targetSpeed;

    NFErr err = m_player.setPlaybackSpeed(speed);
    NTRACE(TRACE_MEDIACONTROL, "pumpingThread, setPlaybackSpeed(%u), %s",
            speed, err.toString().c_str());
    if (!err.ok())
    {
#define VERBOSE
#ifdef VERBOSE
        NTRACE(TRACE_MEDIACONTROL, "pumpingThread: setPlaybackSpeed fails %s",
               err.toString().c_str());
#endif
        return err;
    } else {
        localState.m_deviceNotReadyForPlaybackStartWaitCount = 0;
        localState.m_currSpeed = speed;
        NTRACE(TRACE_MEDIACONTROL, "playbackDevice_->setSpeed: %u succeeds", speed);
        return NFErr_OK;
    }
}

//////////
//
// checkForAudioTrackChange
//
//////////
bool PumpingThread::checkForAudioTrackChange()
{
    // if the previous audio track switch is not finished yet, we would ignore the
    // request until later.
    if (m_playerState->m_waitOnAudioSwitch)
        return false;

    // check to see if it's time to switch tracks
    return m_player.checkForSwitchAudioTrack();
}


void PumpingThread::logGetBlockOutcomes(LocalState &localState)
{
    AdaptiveStreamingPlayer::AvPumpingContext avContext = localState.m_avPumpingContext;
    if(!localState.m_firstAudioGetBlockOutcomeLogged ||
            (avContext.currAudioBlock_.mValid &&
             !localState.m_firstValidAudioGetBlockOutcomeLogged))
    {
        localState.m_firstAudioGetBlockOutcomeLogged = true;
        if(avContext.currAudioBlock_.mValid)
        {
            localState.m_firstValidAudioGetBlockOutcomeLogged = true;
        }
        Log::info(TRACE_MEDIACONTROL,
                "PumpingThread::PumpData getBlocks outcome for audio: "
                "mValid: %d, mPtsInMS: %llu",
                avContext.currAudioBlock_.mValid ? 1 : 0,
                avContext.currAudioBlock_.mPtsInMS);
    }

    if(!localState.m_firstVideoGetBlockOutcomeLogged ||
            (avContext.currVideoBlock_.mValid &&
             !localState.m_firstValidVideoGetBlockOutcomeLogged))
    {
        localState.m_firstVideoGetBlockOutcomeLogged = true;
        if(avContext.currVideoBlock_.mValid)
        {
            localState.m_firstValidVideoGetBlockOutcomeLogged = true;
        }
        Log::info(TRACE_MEDIACONTROL,
                "PumpingThread::PumpData getBlocks outcome for video: "
                "mValid: %d, mPtsInMS: %llu",
                avContext.currVideoBlock_.mValid ? 1 : 0,
                avContext.currVideoBlock_.mPtsInMS);
    }

    AseTimeVal now = AseTimeVal::now();
    if ( localState.m_lastGetBlockReportTime + AseTimeVal::fromMS(STATUS_REPORT_INTERVAL) < now )
    {
        ullong audioPts = avContext.currAudioBlock_.mPtsInMS;
        ullong videoPts = avContext.currVideoBlock_.mPtsInMS;
        NTRACE(TRACE_MEDIACONTROL,
                "Player get block report: audio: %d: %llu ms, video %d: %llu ms"
                ", delivery report: audio: %u:%u, video: %u:%u",
                avContext.currAudioBlock_.mValid ? 1 : 0,
                audioPts,
                avContext.currVideoBlock_.mValid ? 1 : 0,
                videoPts,
                localState.m_deliveredAudioBlocks,
                localState.m_attemptedAudioBlocks,
                localState.m_deliveredVideoBlocks,
                localState.m_attemptedVideoBlocks);
        localState.m_lastGetBlockReportTime = now;
        if ( (audioPts != INVALID_PTS) && (videoPts != INVALID_PTS) )
            localState.m_ptsForLastDeliveredBlock = audioPts > videoPts ? audioPts : videoPts;
    }
}

//////////
//
// pumpData
//
//////////
bool PumpingThread::pumpData(LocalState &localState)
{
    PERFORMANCE_MARKER_SCOPED("pumping.pumpdata");

    NFErr err;

    // If there are stream headers that haven't been parsed yet, feed them
    // to the playback device.
    if (!m_player.feedHeaders())
    {
        // loop back the pipeline to error state
        // (error state should have been set by player)
        return true;
    }

    // If "endOfStream_" is set, no more data block reading is necessary.
    // Otherwise, we should get the next audio and video data blocks for
    // delivery.
    if (!m_playerState->m_audioEndOfStream || !m_playerState->m_videoEndOfStream)
    {
        m_player.getBlocks(localState.m_avPumpingContext);
        logGetBlockOutcomes(localState);
    }

    // for faststart, make sure any drm video block we have has the proper license
    if (m_faststart && localState.m_avPumpingContext.currVideoBlock_.mValid)
    {
        uint32_t manifestIndex =
            localState.m_avPumpingContext.currVideoBlock_.mManifestIndex;
        if ((manifestIndex == 1) && !m_playerState->m_licenseAcquired)
        {
            if (!m_loggedNoDrm)
            {
                Log::info(TRACE_MEDIACONTROL,
                          "pumpData has drm video block but no license");
                m_loggedNoDrm = true;
            }
            return true;
        }
    }

    if (m_loggedNoDrm)
    {
        Log::info(TRACE_MEDIACONTROL, "pumpData resuming, license acquired");
        m_loggedNoDrm = false;
    }

    // If we have no data block to deliver, if the device is full, or if the device is
    // not ready, idle the pipeline for a few ms.
    return feedAudioAndVideoBlocks(localState);
}

//////////
//
// checkPlaybackReady
//
//////////
bool PumpingThread::checkPlaybackReady(LocalState &localState)
{
    if (m_targetSpeed == localState.m_currSpeed)
        return true; // playback ok, nothing to do

    // it gets here when device does not return NFErr_OK on setPlaybackSpeed() and
    // feedBlock() returns pending.
    if (!localState.checkDeviceNotReadyForPlaybackStart(m_targetSpeed))
        return true; // device not ready, but wait a little longer

    NTRACE(TRACE_MEDIACONTROL, "pumpingThread: waited 30 seconds for device to be ready "
           "for playback start. Could change playback speed from %u to %u",
           localState.m_currSpeed, m_targetSpeed);
    Log::warn(TRACE_MEDIACONTROL,
              "pumpingThread: Warning: Device does not become ready to start playback");

    // this is necessary for sequence : err->resumePlay->trickPlay->resume
    NFErr err(NFErr_MC_DevicePlaybackSetSpeedTimeout);
    err.push(NFErr_MC_DevicePlaybackError);

    m_player.reportError(err);

    // notified error, loop back to error state
    return false;
}

//////////
//
// checkEndOfBuffering
//
//////////
bool PumpingThread::checkEndOfBuffering(LocalState &localState)
{
    // Here the pumping thread has tried its best to move data into the playback
    // device. So, if playback is halted and "bufferingComplete_" is set (by the
    // stream manager), it's time to end buffering state.
    if (m_playerState->m_halted && (m_faststart || m_playerState->m_licenseAcquired) &&
        ((m_playerState->m_audioEndOfStream && m_playerState->m_videoEndOfStream)
         || m_playerState->m_bufferingComplete))
    {
        uint32_t currentPts = INVALID_PTS;
        if (m_player.havePts_)
            currentPts = m_player.currentPts_;
        uint32_t minimumDataInDevice = m_player.minimumDataInDevice_;

        if (localState.checkBlocksDelivered(currentPts, minimumDataInDevice))
        {
            NTRACE(TRACE_MEDIACONTROL, "checkEndOfBuffering, delivered %u:%u video blocks and %u:%u audio blocks",
                    localState.m_deliveredVideoBlocks, localState.m_attemptedVideoBlocks,
                    localState.m_deliveredAudioBlocks, localState.m_attemptedAudioBlocks);
            m_player.endBuffering();
        }
        else // no blocks were delivered
        {
            if (localState.checkBufferingErrorWaitCount())
            {
                NTRACE(TRACE_MEDIACONTROL, "buffering error: endOfStream_ = %s",
                       m_playerState->m_audioEndOfStream &&
                       m_playerState->m_videoEndOfStream ? "true" : "false");
                Log::warn(TRACE_MEDIACONTROL, "Device reports underflow "
                          "but has no space for feeding data");
                NTRACE(TRACE_MEDIACONTROL,
                       "Audio device returned \"pending\" for the last %u"
                       " feedData() calls. Video device returned \"pending\" for the "
                       " last %u feedData() calls.",
                       localState.m_numSuccessivePendingReturnsFromAudioFeedData,
                       localState.m_numSuccessivePendingReturnsFromVideoFeedData);

                NFErr err(NFErr_MC_DeviceBufferingWaitCountExceeded);
                err.push(NFErr_MC_DevicePlaybackError);
                m_player.reportError(err);

                if (localState.checkUnderflowAttempts())
                {
                    Log::warn(TRACE_MEDIACONTROL,
                              "MediaControl does not attempt to deliver a/v block "
                              "before device reports underflow");
                }

                return false;  // loop back for error state
            }
        }
    }

    return true;
}

//////////
//
// feedAudioAndVideoBlocks
//
// Returns true if the pumpingLoop should continue from the top, false if the
// pumping loop should fall through, check for end of buffering and wait before
// looping again. Returns false if there is no block to feed to the playback
// device (possibly due to end of stream) or if the playback device returns
// NFErr_Pending.
//
//////////
bool PumpingThread::feedAudioAndVideoBlocks(LocalState& localState)
{
    bool shouldContinue = false;
    NFErr err;

    AseTimeVal now = AseTimeVal::now();
    IAdaptiveStreamingPlayer::AvPumpingContext avContext = localState.m_avPumpingContext;
    if ( localState.m_lastFeedBlockReportTime + AseTimeVal::fromMS(STATUS_REPORT_INTERVAL) < now )
    {
        NTRACE(TRACE_MEDIACONTROL,
                "feedAudioAndVideoBlocks status: audio: %u:%u:%u, video: %u:%u"
                ", delivery report: audio:%u:%u, video:%u, %u",
                m_playerState->m_suspendAudioDataFeed,
                avContext.audioReadPTS_,
                avContext.currAudioBlock_.mValid,
                avContext.videoReadPTS_,
                avContext.currVideoBlock_.mValid,
                localState.m_deliveredAudioBlocks,
                localState.m_attemptedAudioBlocks,
                localState.m_deliveredVideoBlocks,
                localState.m_attemptedVideoBlocks);

        localState.m_lastFeedBlockReportTime = now;
    }

    if (!m_playerState->m_suspendAudioDataFeed &&
        avContext.audioReadPTS_ != MAX_PTS &&
        avContext.currAudioBlock_.mValid)
    {
        err = feedDataToPlaybackDevice(localState, IPlaybackDevice::AUDIO);
        if (err.ok())
        {
            shouldContinue = true;
        }
        else
        {
            shouldContinue =
                handleFeedDataErrorCode(err, localState, IPlaybackDevice::AUDIO);
        }
    }
    if (avContext.videoReadPTS_ != MAX_PTS &&
        avContext.currVideoBlock_.mValid)
    {
        err = feedDataToPlaybackDevice(localState, IPlaybackDevice::VIDEO);
        if (err.ok())
        {
            shouldContinue = true;
        }
        else
        {
            shouldContinue |=
                handleFeedDataErrorCode(err, localState, IPlaybackDevice::VIDEO);
        }
    }
    return shouldContinue;
}

//////////
//
// feedDataToPlaybackDevice
//
//////////
NFErr PumpingThread::feedDataToPlaybackDevice(LocalState& localState,
                                              IPlaybackDevice::StreamType streamType)
{
    IAdaptiveStreamingPlayer::MediaEntity* blockToDeliver;

    NFErr err;

    if(streamType == IPlaybackDevice::AUDIO)
    {
        ++localState.m_attemptedAudioBlocks;
        blockToDeliver = &localState.m_avPumpingContext.currAudioBlock_;
    } else {
        ++localState.m_attemptedVideoBlocks;
        blockToDeliver = &localState.m_avPumpingContext.currVideoBlock_;
    }

    uint32_t manifestIndex = blockToDeliver->mManifestIndex;
    shared_ptr<AdaptiveStreamingPlayer::ManifestMetaData> mmd =
        m_player.manifestData_[manifestIndex];

    assert(blockToDeliver->mTrackIndex < mmd->firstStreamIndexInTrack_.size());

    uint32_t streamIndex =
        mmd->firstStreamIndexInTrack_[blockToDeliver->mTrackIndex] +
        blockToDeliver->mStreamIndex;

    // if this is a new stream header deliver it
    if (blockToDeliver->mNewStreamHeader.get())
    {
        m_player.feedStreamHeader(streamType, streamIndex,
                                  blockToDeliver->mManifestIndex,
                                  blockToDeliver->mNewStreamHeader);
        blockToDeliver->mValid = false;
        return NFErr_OK;
    }

    if (blockToDeliver->mDataBlock.get() == NULL)
        return NFErr_OK; // no data, continue pipeline

    // deliver the data to the playback device
    err = m_player.feedBlock(streamType,
                             streamIndex,
                             blockToDeliver);

    if (err.ok())
    {
        localState.deliveredBlock(streamType);

        // If the data block is successfully delivered, its device memory
        // buffer will be managed by the playback device. We should detach
        // the device memory buffer here.
        blockToDeliver->mValid = false;

        return NFErr_OK; // block delivered, continue pipeline
    }

    return err;
}

//////////
//
// handleFeedDataErrorCode
//
//////////
bool PumpingThread::handleFeedDataErrorCode(NFErr& err, LocalState& localState,
                                            IPlaybackDevice::StreamType streamType)
{
    if (err.contains(NFErr_Pending))
    {
        localState.pendingBlock(streamType);

        // The internal pipeline of playback device is full.
        return false;
    }

    if (err.contains(NFErr_NotReady))
    {
        // The playback device is not ready to receive data.
        wait(WAIT_WHILE_FEEDING);
        return true;
    }

    if (err.contains(NFErr_Bad) ||
        err.contains(NFErr_NotAllowed) ||
        err.contains(NFErr_ArrayOutOfBoundary) ||
        err.contains(NFErr_DRMFailed))
    {
        // Playback error, pumping thread should stall
        return true;
    }

    Log::warn(TRACE_MEDIACONTROL, "Unexpected error from feedData: %s",
              err.toString().c_str());
    assert(false); // unexpected.
    return false;
}
