/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __IPLAYBACK_REPORTER_H__
#define __IPLAYBACK_REPORTER_H__

#include <nrd/config.h>

#include <nrdase/IStreamingReporter.h>
#include <nrdase/ITransportReporter.h>

#include "IMediaPipelineReporter.h"
#include <nrd/IPlaybackDevice.h>


namespace netflix {
namespace mediacontrol {


class IPlaybackReporter:virtual public ase::ITransportReporter,
                        virtual public ase::IStreamingReporter,
                        virtual public IMediaPipelineReporter
{


public:
    virtual ~IPlaybackReporter() {}

    virtual void setLicenseTime(ullong licenseTimeMS) = 0;
    virtual void setPlaybackQualityStats(
        const device::IPlaybackDevice::PlaybackQualityStats& audioStats,
        const device::IPlaybackDevice::PlaybackQualityStats& videoStats) = 0;
};

}}

#endif // __PLAYBACK_REPORTER_H__
