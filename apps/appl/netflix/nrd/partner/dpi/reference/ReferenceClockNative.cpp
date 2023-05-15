/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "ReferenceClockNative.h"
#include <nrd/IElementaryStreamPlayer.h>
#include <nrd/IPlaybackDevice.h>
#include <nrd/DeviceConstants.h>
#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/ScopedMutex.h>

using namespace netflix::device::esplayer;
using namespace netflix;


ReferenceClockNative::ReferenceClockNative() :
  speed_(IPlaybackDevice::PAUSED_SPEED),
  lastUpdatedPTS_(INVALID_TIMESTAMP),
  clockAtLastPtsUpdate_(INVALID_TIMESTAMP),
  latestAudioPTS_(INVALID_TIMESTAMP),
  latestVideoPTS_(INVALID_TIMESTAMP),
  mutex_(ESP_REFERENCECLOCK_MUTEX, "ReferenceClock Mutex")
{
}

void ReferenceClockNative::setSpeed(uint32_t speed)
{
    ScopedMutex cs(mutex_);
    speed_ = speed;
}

int64_t ReferenceClockNative::getCurrPresentationTime()
{
    ScopedMutex cs(mutex_);
    if(lastUpdatedPTS_ == INVALID_TIMESTAMP || clockAtLastPtsUpdate_ == INVALID_TIMESTAMP)
    {
        return INVALID_TIMESTAMP;
    }

    return lastUpdatedPTS_ + (((getSystemClock() - clockAtLastPtsUpdate_) * speed_) >>
                              IPlaybackDevice::NORMAL_SPEED_Po2);
}

void ReferenceClockNative::updateCurrPresentationTime(int64_t timestamp)
{
    ScopedMutex cs(mutex_);
    lastUpdatedPTS_ = timestamp;
    clockAtLastPtsUpdate_ = getSystemClock();
}

int64_t ReferenceClockNative::getAudioPresentationTime()
{
    ScopedMutex cs(mutex_);
    return latestAudioPTS_;
}

void ReferenceClockNative::updateAudioPresentationTime(int64_t timestamp)
{
    ScopedMutex cs(mutex_);
    latestAudioPTS_ = timestamp;
}

int64_t ReferenceClockNative::getVideoPresentationTime()
{
    ScopedMutex cs(mutex_);
    return latestVideoPTS_;
}

void ReferenceClockNative::updateVideoPresentationTime(int64_t timestamp)
{
    ScopedMutex cs(mutex_);
    latestVideoPTS_ = timestamp;
}

inline int64_t ReferenceClockNative::getSystemClock() const
{
    return Time::mono().val();
}

uint32_t ReferenceClockNative::getSpeed()
{
    ScopedMutex cs(mutex_);
    return speed_;
}

void ReferenceClockNative::invalidateCurrPresentationTime()
{
    ScopedMutex cs(mutex_);
    lastUpdatedPTS_ = INVALID_TIMESTAMP;
}

void ReferenceClockNative::invalidateAudioPresentationTime()
{
    ScopedMutex cs(mutex_);
    latestAudioPTS_ = INVALID_TIMESTAMP;
}

void ReferenceClockNative::invalidateVideoPresentationTime()
{
    ScopedMutex cs(mutex_);
    latestVideoPTS_ = INVALID_TIMESTAMP;
}
