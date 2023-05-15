/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __PUMPING_THREAD_H__
#define __PUMPING_THREAD_H__

#include "McThread.h"

#include <nrd/IPlaybackDevice.h>

namespace netflix {

namespace mediacontrol {

class AdaptiveStreamingPlayer;
class PlayerState;
class IPlaybackReporter;

class PumpingThread : public McThread
{
public:
    PumpingThread(AdaptiveStreamingPlayer &player,
                  shared_ptr<PlayerState> playerState,
                  shared_ptr<IPlaybackReporter> playbackReporter,
                  bool faststart);
    virtual ~PumpingThread() {}

    //================================================================================
    // Functions used to control the operating state of pumping thread.
    //================================================================================
    void startDataPumping();
    bool stopDataPumping();

    /**
     * Request a new playback speed, which will be honored by the pumping thread.
     *
     * @param[in] speed the new playback speed, which is specified as a fixed
     *            point value. IPlaybackDevice::NORMAL_SPEED (65536) is for
     *            normal speed playback, and IPlaybackDevice::PAUSED_SPEED (0)
     *            is to pause playback.
     */
    void setPlaybackSpeed(uint32_t speed);

private:
    class LocalState;

    virtual void Run();

    /**
     * Transition STOP_PUMPING
     */
    void stopPumping(LocalState &localState);

    /**
     * Generates the challenge and provides the license if necessary
     */
    NFErr handleLicense();

    /**
     * Handles generating the challenge
     */
    NFErr generateChallenge();

    /**
     * Handles providing the challenge to the LicenseHandler
     */
    NFErr provideChallenge();

    /**
     * Checks for any changes tp speed on playback device
     */
    NFErr checkForSpeedChange(LocalState &localState);

    /**
     * Checks for any audio track changes
     * @return true if track changed
     */
    bool checkForAudioTrackChange();

    /**
     * Actually pumps data from streaming manager to playback device
     * @return false if the pipeline is full, otherwise true
     */
    bool pumpData(LocalState &localState);

    /**
     * Logs the first currAudioBlock's and currVideoBlock's mValid and mPtsInMS
     * fields.  Only logs for the first blocks gotten after start or rebuffer.
     * If the first block gotten has mValid=false, log again the first time
     * mValid is true.  Does this independently for audio and video.
     */
    void logGetBlockOutcomes(LocalState &localState);


    /**
     * Make sure the playback device was ready for playback
     * @return false if playback device not ready and error reported
     */
    bool checkPlaybackReady(LocalState &localState);

    /**
     * check for the end of buffering
     * @return false if no blocks were attempted to be delivered
     */
    bool checkEndOfBuffering(LocalState &localState);

    /**
     * Tries to feed and audio and a video block to the DPI.
     *
     * @return true if the pumping loop should continue from the top. Returns
     * false if the pumping loop should fall through, check for end of buffering
     * and wait for a timeout before looping again.  Returns false if there is
     * no block to feed to the playback device (possibly due to end of stream)
     * or if the playback device returns NFErr_Pending for both the audio and
     * video block, this means playback device doesn't currently have any space
     * for another block and the pumping loop should try again later.
     */
    bool feedAudioAndVideoBlocks(LocalState& localState);

    /**
     * Helper function to feed the next audio or next video block to the DPI.
     *
     * @return NFErr_OK if a header or media fragment block are successfully
     * delivered. Otherwise, Will return the IPlaybackDevice::feedData() NFErr
     * error.
     *
     */
    NFErr feedDataToPlaybackDevice(LocalState& localState,
                                   device::IPlaybackDevice::StreamType streamType);

    /**
     * Handles IPlaybackDevice's feedData error.
     * Returns false if the stream is NFErr_Pending.
     */
    bool handleFeedDataErrorCode(NFErr& err, LocalState& localState,
                                 device::IPlaybackDevice::StreamType streamType);


private:
    static const size_t PUMPING_THREAD_STACK_SIZE = 32768;

    enum State
    {
        STOP_PUMPING,
        PUMPING_STOPPED,
        PUMPING
    };

private:
    State m_state;
    AdaptiveStreamingPlayer &m_player;
    shared_ptr<PlayerState> m_playerState;

    shared_ptr<IPlaybackReporter> m_playbackReporter;

    bool m_faststart;
    bool m_challengeGenerated;
    bool m_challengeProvided;
    bool m_loggedNoDrm;

    volatile uint32_t m_targetSpeed;

    uint32_t m_slowPumpingEnterThreshold;
    uint32_t m_slowPumpingExitThreshold;
    uint32_t m_sleepDuringSlowPumping; // ms
};

}} // namespace netflix::mediacontrol

#endif //  __PUMPING_THREAD_H__
