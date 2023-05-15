/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef REFERENCECLOCKNATIVE_H
#define REFERENCECLOCKNATIVE_H

#include <nrddpi/Common.h>
#include <nrdbase/Exportable.h>
#include <nrdbase/Mutex.h>

/** @file ReferenceClockNative.h - The reference application uses the
 * ReferenceClockNative class to track audio and video playback timestamps so
 * that audio and video players can play in synchronization.
 *
 * A device partner may use or modify this header and accompanying .cpp file as
 * needed.
 */

namespace netflix {
namespace device {
namespace esplayer {


/**
 * @class ReferenceClockNative ReferenceClockNative.h
 * @brief System reference-clock who is in charge of timestamps and speed
 */
class NRDP_EXPORTABLE ReferenceClockNative
{
public:
    /**
     * Constructor.
     */
    ReferenceClockNative();

    /**
     * Set speed setting.
     * The speed setting in units of 1/65536 of the normal playback speed.
     *
     * @param[in] speed the speed setting to be set.
     */
    void setSpeed(uint32_t speed);

    /**
     * Get the current speed setting.
     * The speed setting in units of 1/65536 of the normal playback speed.
     *
     * @return the current speed setting.
     */
    uint32_t getSpeed();

    /**
     * Get the current presentation time. Used by the AV-Sync "slave" or onlookers.
     * It is the accurate extrapolation of what time position the current presentation is at.
     *
     * @return the current presentation time.
     */
    int64_t getCurrPresentationTime();

    /**
     * Update the current presentation time. Used by the AV-Sync "master".
     *
     * @param[in] timestamp the presentation time to be updated as current.
     */
    void updateCurrPresentationTime(int64_t timestamp);

    /**
     * Invalidate the current presentation time. Used by the AV-Sync "master" or
     * the central controller.
     */
    void invalidateCurrPresentationTime();

    /**
     * Get the latest audio presentation timestamp.
     * It is the latest updated time position of audio presentation.
     *
     * @return the latest audio presentation timestamp.
     */
    int64_t getAudioPresentationTime();

    /**
     * Update the latest audio presentation timestamp.
     *
     * @param[in] timestamp the audio presentation timestamp to be updated as latest.
     */
    void updateAudioPresentationTime(int64_t timestamp);

    /** Invalidate the latest audio presentation timestamp. */
    void invalidateAudioPresentationTime();

    /**
     * Get the latest video presentation timestamp.
     * It is the latest updated time position of video presentation.
     *
     * @return the latest video presentation timestamp.
     */
    int64_t getVideoPresentationTime();

    /**
     * Update the latest video presentation timestamp.
     *
     * @param[in] timestamp the video presentation timestamp to be updated as latest.
     */
    void updateVideoPresentationTime(int64_t timestamp);

    /** Invalidate the latest video presentation timestamp. */
    void invalidateVideoPresentationTime();

private:

    // The current speed setting in units of 1/65536 of the normal playback speed.
    uint32_t speed_;

    // The latest updated presentation time.
    int64_t lastUpdatedPTS_;

    // The system clock when the latest presentation time was updated.
    int64_t clockAtLastPtsUpdate_;

    // The latest audio presentation time.
    int64_t latestAudioPTS_;

    // The latest video presentation time.
    int64_t latestVideoPTS_;

    Mutex mutex_;

private: // helpers
    int64_t getSystemClock() const;
};

} // namespace esplayer
} // namespace device
} //

#endif // DEVICE_REFERENCECLOCK_H
