/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __I_MEDIAPIPELINE_REPORTER_H__
#define __I_MEDIAPIPELINE_REPORTER_H__

#include <nrdbase/tr1.h>
#include <nrdbase/NFErr.h>

#include <nrdase/AseTimeStamp.h>

namespace netflix {
namespace mediacontrol {

/**
 * @class IMediaPipelineReporter
 * @brief IMediaPipelineReporter is notified of any playback related event
 */
class IMediaPipelineReporter
{
public:
    /** Playback State */
    enum PlaybackState
    {
        PAUSED_STATE,
        UNPAUSED_STATE
    };

    /** Display mode */
    enum DisplayMode
    {
        INTERNAL_DISPLAY,
        EXTERNAL_DISPLAY
    };

    virtual ~IMediaPipelineReporter() {}

    typedef shared_ptr<IMediaPipelineReporter> IMediaPipelineReporterPtr;

    /**
     * Change to the playback state
     * XXX: temporary api
     */
    virtual void playbackStateChange(const ase::AseTimeStamp &movieTime,
                                     PlaybackState state) = 0;

    /**
     * The playback starts successfully.
     * @param[in] additionalInfo: addition info for 'startplay'
     * @param[in] eventTime: optional parameter which indicates the event time
     */
    virtual void playbackStarted(std::string const& additionalInfo, Time const& eventTime = Time() ) = 0;

    /**
     * The playback failed for some reason
     * @param[in] err the NFErr of the failure
     */
    virtual void playbackFailure(NFErr err) = 0;

    /**
     * Notified when a new stream being played
     * @param[in] manifestIndex the index of the manifest
     * @param[in] streamIndex the index of the new stream.
     * @param[in] movieTime the media time where the new stream becomes effective.
     */
    virtual void playbackStreamChange(int32_t manifestIndex,
                                      int32_t trackIndex, int32_t streamIndex,
                                      const ase::AseTimeStamp &movieTime) = 0;

    /**
     * Notified when the media pipeline repositions the playback stream (seeks)
     * @param[in] movieTime the new play position of the stream
     */
    virtual void playbackReposition(const ase::AseTimeStamp &movieTime) = 0;

    /**
     * updatePlaybackPts will be called frequently, e.g. every 500 ms, to maintain
     * an accurate estimate.
     * @param[in] movieTime the current playback pts
     * @param[in] vcdnid the id of the cdn serving the video block
     * @param[in] acdnid the id of the cdn serving the audio block
     */
    virtual void updatePlaybackPts(const ase::AseTimeStamp &movieTime,
                                   uint32_t vcdnid, uint32_t acdnid) = 0;

    /**
     * Notified when the playback is stopped either intentionally or unintentionally
     * @param[in] reason: the reason for stopping the playback, e.g. stopped, rebuffer,
     *     pause, or ended
     * @param[in] movieTime the current playback pts
     * @param[in] eventTime: optional parameter which indicates the event time
    */
    virtual void playbackStopped(NFErr reason, const ase::AseTimeStamp &movieTime,
                                 Time const& eventTime = Time() ) = 0;

    /**
     * Notifiled when the playback session is closed
     * @param[in] reason: the reason for closing the session, e.g. ended, error
     * @param[in] movieTime the current playback pts
     */
    virtual void playbackClosed(NFErr reason, const ase::AseTimeStamp &movieTime) = 0;

    /**
     * Set the network parameters for logging
     *
     * @param[in] carrier The carrier in use
     * @param[in] countryCode The country code in use
     * @param[in] networkCode The network code in use
     * @param[in] networkType The network type (GSM, CDMA, WiMax, WiFi, Wired)
     * @param[in] networkSpec The network spec (2g, 3g, 4g, a, b, g, n, 10Mbps, etc)
     * @param[in] lastBytesReceived The bytes received on the last network (optional)
     */
/*    virtual void setNetworkParameters(const std::string &carrier,
                                      const std::string &countryCode,
                                      const std::string &networkCode,
                                      const std::string &networkType,
                                      const std::string &networkSpec,
                                      uint32_t lastBytesReceived = 0) = 0;
*/

};

typedef IMediaPipelineReporter::IMediaPipelineReporterPtr IMediaPipelineReporterPtr;

}}

#endif // __I_MEDIAPIPELINE_REPORTER_H__
