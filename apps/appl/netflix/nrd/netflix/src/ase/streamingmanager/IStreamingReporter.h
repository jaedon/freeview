/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_STREAMING_REPORTER_H
#define I_STREAMING_REPORTER_H

#include <nrdbase/AseErrorCodes.h>
#include <nrdase/AseTimeStamp.h>
#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase {

class ILocationSetMonitor;

/**
 * @class IStreamingReporter
 * @brief streaming event listener is notified of streaming event for reporting purpose
 */

class IStreamingReporter
{
public:
    virtual ~IStreamingReporter() {}

    typedef shared_ptr<IStreamingReporter> IStreamingReporterPtr;

    /**
     * Deprecated
     * Notified that the open is completed.
     * @param[in] duration: the duration of the content in ms
     * @param[in] gopSize: gop size
     */
    virtual void openCompleted(uint32_t duration, uint32_t gopSize) = 0;

    /**
     * Deprecated
     * update the content duration
     * @param[in] duration
     */
    virtual void updateContentDuration(uint32_t duration) = 0;

    /**
     * Deprecated
     * Notified that given manifest is selected
     */
    virtual void notifyManifestSelected( uint32_t manifestIndex ) = 0;

    /**
     * Notified when the Streaming Manager starts buffering.
     * It's the time point after the streaming manager downloads the first media header.
     */
    virtual void bufferingStarted() = 0;

    /**
     * Notified when the skips forward/backward starts
     */
    virtual void skipStarted() = 0;

    /**
     * To notify of the streaming manager's completion of data buffering.
     *
     * This should be equivalent to the HTML5 HAVE_ENOUGH readyState
     * @param[in] actualStartPts the actual startplay/resumeplay pts in milliseconds
     * @param[in] predictedBufferingTime Estimated buffering time in milliseconds
     * @param[in] initialBW Estimated inital bandwidth in kbps
     * @param[in] actualBW Actaul bandwidth in kbps
     */
    virtual void bufferingComplete(uint32_t actualStartPts,
                                   ullong predictedBufferingTime,
                                   uint32_t initialBW, uint32_t actualBW) = 0;

    /**
     * Deprecated
     * To notify of the streaming manager's progress in data buffering.
     *
     * @param[in] totalBufferingTime the total amount of buffering requirement
     *            (measured in time, ms).
     * @param[in] currentBufferingTime the current buffering level (measured in time, ms).
     */
    virtual void bufferingStatus(uint32_t totalBufferingTime,
                                 uint32_t currentBufferingTime) = 0;

    /**
     * To notify of an irrecoverable streaming failure for the session.
     * If the message is received before the startplay, it triggers a 'startplay' message with failure,
     * otherwise it triggers an 'endplay' message with 'endreason' = 'error'
     * @param[in] manifestIndex the index of the manifest,
     * @param[in] trackIndex the index of the track. -1 means the failure was not caused by a specific track
     * @param[in] streamIndex the index of the stream. -1 means the failure was not caused by a specific stream
     * @param[in] error the error code to explain the failure.
     * @param[in] httpErr the http error code
     * @param[in] errorMsg an optional error message to explain the failure
     */
    virtual void streamingFailure(int32_t manifestIndex,
                                  int32_t trackIndex, int32_t streamIndex,
                                  AseErrorCode error, int32_t httpErr,
                                  const std::string &errorMsg) = 0;

    virtual void reportStreamingServerFailure(uint32_t cdnId) = 0;

    virtual void streamingFailureReasons(AseErrorCode networkError, int32_t httpErr) = 0;

    /**
     * Notified when a new stream gets selected.
     * @param[in] manifestIndex the index of the manifest
     * @param[in] trackIndex the index of the track whose stream has changed.
     * @param[in] streamIndex the index of the selected stream.
     * @param[in] movieTime the media time where the selected stream becomes effective.
     * @param[in] bandwidth short term bandwidth estimate
     * @param[in] longtermBw long term bandwidth estimate
     * @param[in] rebuffer whether the player will rebuffer with the selected stream
     */
    virtual void streamSelected( int32_t manifestIndex,
                                 int32_t trackIndex, int32_t streamIndex,
                                 AseTimeStamp const& movieTime,
                                 uint32_t bandwidth,
                                 uint32_t longtermBw,
                                 uint32_t rebuffer) = 0;

    /**
     * Update the current streaming time.
     * updateStreamingPts is called frequently, e.g. every 500 ms, to maintain an accurate steaming timestamp.
     * This is can be used to compute the media's buffer level
     * @param[in] manifestIndex the index of the manifest
     * @param[in] trackIndex the index of the track
     * @param[in] movieTime the current streaming pts
     */
    virtual void updateStreamingPts( int32_t manifestIndex, int32_t trackIndex,
                                     AseTimeStamp const& movieTime ) = 0;

    /**
     * update the current a/v buffer level
     * @param[in] abuflbytes: audio buffer in bytes
     * @param[in] vbuflbytes: video buffer in bytes
     * @param[in] predictedFutureRebuffers: the predicted number of future rebuffers
     */
    virtual void updateBufferLevel( uint32_t abuflbytes, uint32_t vbuflbytes,
                                    uint32_t predictedFutureRebuffers,
                                    uint32_t currentBandwidth) = 0;

    /**
     * update detailed streaming statistics
     * @param[in] streaming stat
     */
    virtual void updateStreamingStat( shared_ptr<StreamingStatReport> pStreamingStat) = 0;

    /**
     * NOtified when a new server is selected
     * This triggers to log a 'cdnsel' message.
     * @param[in] selcdnid: selected server/cdn id
     * @param[in] selcdnname: selected server/cdn name
     * @param[in] selcdnrtt:
     * @param[in] selcdnbw:
     * @param[in] domainbwdata: map to cdnbwdata. It tracks bandwidth info of different locations
     * @param[in] testreason
     * @param[in] selreason
     * @param[in] fastselthreshold
     */
    virtual void serverSelected( uint32_t selcdnid,
                                 std::string const& selcdnname,
                                 uint32_t selcdnrtt,
                                 uint32_t selcdnbw,
                                 std::string const& domainbwdata,
                                 std::string const& testreason,
                                 std::string const& selreason,
                                 uint32_t fastselthreshold ) = 0;

    /**
     * Notified that the audio track switch is started
     * @param[in] oldLangCode: the old language code
     * @param[in] oldNumChannels: the number of channels in the old audio track
     * @param[in] newLangCode: the new language code
     * @param[in] newNumChannels: the number of channels in the new audio track
     */
    virtual void audioTrackSwitchStarted(std::string const& oldLangCode, uint32_t oldNumChannels,
                            std::string const& newLangCode, uint32_t newNumChannels) = 0;


    /**
     * Notified that the audio track switch is completed
     */
    virtual void audioTrackSwitchComplete() = 0;

    /**
     * Deprecated
     * Notified that the given mediatype's buffer is flushed
     * So it's safe to flush the buffer at the PlaybackDevice when
     * we switch a media track
     */
    virtual void flushComplete(MediaType mediaType) = 0;

    /**
     * Deprecated
     * Report the number of connection selected based on rtt
     * @param[in] origmaxnum original maximum number of connections
     * @param[in] actualmaxnum actual maximum number of connections
     * @param[in] rtt rtt in ms
     */
    virtual void connectionSelected1( uint32_t origmaxnum, uint32_t actualmaxnum, uint32_t rtt ) = 0;

    /**
     * Deprecated
     * Report the number of connections selected by the auto tuning algorithm
     * @param[in] defaultnumconn: default number of connections
     * @param[in] selnumconn: selected number of connections
     * @param[in] maxnumconn: maximum number of connections
     */
    virtual void connectionSelected2( uint32_t defaultnumconn, uint32_t selnumconn, uint32_t maxnumconn ) = 0;


    /**
     * Deprecated
     * Notified when the video bitrate selection completed
     */
    virtual void videoBitrateSelectionCompleted() = 0;

    /**
     * Notified when a key startplay event completed
     */
    virtual void reportKeyStartplayEvent( std::string const& eventName ) = 0;

    /**
     * Notified when the player disables http pipelining
     */
    virtual void pipeliningDisabled() = 0;

    enum ServerSelectionReason
    {
        SERVER_UNKNOWN = 0,
        SERVER_STARTUP,
        SERVER_PROBE,
        SERVER_PROBEDONE,
        SERVER_PROBEABORTED,
        SERVER_PERFORMANCE,
        SERVER_BITRATE,
        SERVER_SERVERFAILOVER,
        SERVER_LOCATIONFAILOVER,
        SERVER_TOLOWGRADE,
        SERVER_FROMLOWGRADE
    };

    /** Report a - possibly temporary - server switch
     *
     *  @param cdnid [in] The server that is being selected, a.k.a server id
     *  @param pLocationSetMonitor [in] The location set monitor for the location
     *  @param reason [in] The reason for the change
     */
    virtual void serverSwitch(
                        uint32_t                        cdnid,
                        shared_ptr<ILocationSetMonitor> pLocationSetMonitor,
                        ServerSelectionReason           reason,
                        uint32_t                        bitrate ) = 0;

    /** Report the token from the file header - this may be an individualized field
     *  in the file header enabling proxy detection
     *
     *  @param filetoken [in] The file token
     */
    virtual void reportFileToken( std::vector<unsigned char> const& filetoken ) = 0;
};

typedef IStreamingReporter::IStreamingReporterPtr IStreamingReporterPtr;

}}
#endif
