/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "PlayerState.h"

using namespace netflix::mediacontrol;

PlayerState::PlayerState()
    : m_playbackDeviceOpen(false),
      m_videoPlayerInitialized(false),
      m_audioPlayerInitialized(false),
      m_drmHeaderDelivered(false),
      m_bufferingComplete(false),
      m_drmHeaderParsed(false),
      m_licenseAcquired(false),
      m_licenseNeedClear(false),
      m_waitOnAudioSwitch(false),
      m_suspendAudioDataFeed(false),
      m_waitOnVideoBitrateSelection(false),
      m_halted(false),
      m_audioEndOfStream(false),
      m_videoEndOfStream(false),
      m_error(false)
{
}

void PlayerState::resetOnOpen()
{
    m_playbackDeviceOpen = false;
    m_videoPlayerInitialized = false;
    m_audioPlayerInitialized = false;
    m_drmHeaderDelivered = false;
    m_bufferingComplete = false;
    m_drmHeaderParsed = false;

    m_halted = true;                // playback is halted.
    m_audioEndOfStream = false;     // streaming is restarted.
    m_videoEndOfStream = false;

    m_licenseAcquired = false;      // no license stored
    m_licenseNeedClear = false;

    m_error = false;
}

void PlayerState::resetOnPlay(bool resetBufferingComplete)
{
    m_halted = true;
    m_audioEndOfStream = false;
    m_videoEndOfStream = false;
    m_waitOnAudioSwitch = false;
    m_suspendAudioDataFeed = false;
    m_waitOnVideoBitrateSelection = false;
    if (resetBufferingComplete)
        m_bufferingComplete = false;
}
