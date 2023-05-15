/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __PLAYERSTATE_H__
#define __PLAYERSTATE_H__

namespace netflix {
namespace mediacontrol {

// State parameters shared between AdaptiveStreamingPlayer and PumpingThread
class PlayerState
{
public:
    PlayerState();

    // reset the variables to state
    void resetOnOpen();
    void resetOnPlay(bool resetBufferingComplete);

public:
    volatile bool m_playbackDeviceOpen;

    volatile bool m_videoPlayerInitialized;
    volatile bool m_audioPlayerInitialized;

    volatile bool m_drmHeaderDelivered;

    volatile bool m_bufferingComplete;
    volatile bool m_drmHeaderParsed;
    volatile bool m_licenseAcquired;
    volatile bool m_licenseNeedClear;

    volatile bool m_waitOnAudioSwitch;
    volatile bool m_suspendAudioDataFeed;
    volatile bool m_waitOnVideoBitrateSelection;

    volatile bool m_halted;
    volatile bool m_audioEndOfStream;
    volatile bool m_videoEndOfStream;

    volatile bool m_error;
};

}} // namespace netflix::mediacontrol

#endif // __PLAYERSTATE_H__
