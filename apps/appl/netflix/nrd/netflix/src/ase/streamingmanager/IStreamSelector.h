/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef I_STREAM_SELECTOR_H
#define I_STREAM_SELECTOR_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/ITrackInfo.h>
#include <nrdase/ITrackBufferInfo.h>

#include <nrdase/Manifest.h>

namespace netflix {
namespace ase {

class IAseConfigParameter;
typedef shared_ptr<IAseConfigParameter> IAseConfigParameterPtr;

/**
 * @class IStreamSelector
 * @brief An interface for stream selection, a.k.a rate adaptation
 *
 * An object implementing this interface must be provided to perform
 * stream selection. The object is "stateless" - it is queried each
 * time a decision is needed and provided at this time with all the
 * information available to make the decision. It should make a decision
 * based on the information provided (i.e. there is no provision at
 * present for it to say 'more information required').
 *
 * The input information consists of:
 *      History
 *          What has happened on the network in the past, as reported
 *          through the INetworkMonitor interface.
 *      Current state
 *          The amount of currently buffered data for each track,
 *          its bitrate profile and the available buffer space. Also the
 *          currently selected stream for each track.
 *      Available choices
 *          The possible tracks and streams from which the next fragment
 *          to be downloaded could be chosen and the bitrate profile
 *          of those streams into the future.
 *
 * The task of the IStreamSelector is to use the history to make a
 * prediction of future bandwidth and to analyse which of the available
 * choices will result in the best future user experience, based on this
 * prediction.
 *
 * The output is the selected track and stream for the next request.
 */
class IStreamSelector
{
public:
    typedef shared_ptr<IStreamSelector> IStreamSelectorPtr;

    virtual void configure( IAseConfigParameterPtr pConfig ) = 0;

    enum BufferingStatus
    {
        // The buffer and recent bandwidth are below the level required
        // for successful uninterrupted playback
        BUFFER_LOW = 0,

        // The buffer and recent bandwidth are such that playout
        // is expected to continue uninterrupted
        BUFFER_HAVE_ENOUGH
    };

    enum StreamFeasibility
    {
        // The heuristics predicts rebuffer if streaming this stream.
        UNFEASIBLE = 0,

        // The heuristics does not predict rebuffer if streaming this stream.
        FEASIBLE,

        // The heuristics can not predict whether or not rebuffer if streaming this stream.
        UNKNOWN_FEASIBILITY
    };

    /** select the stream for the next downloading
     * @param[in] bitrateSwitchFreeze: true then freeze the bitrate switch
     * @param[in] trackId: id of the selected track
     * @param[in] currentPlayoutRate The current playout rate
     * @param[in] currentPlayoutTime The current playout time
     * @param[in] trackCurrentInfo Buffer state of currently playing tracks
     * @param[in] availableBuffer The available buffer size in bytes
     * @param[in] trackFutureInfo Future data rate profiles of currently playing tracks
     * @param[out] streamFeasibility: whether the stream is feasible
     * @param[out] streamId: id of the selected stream, -1 is invalid id
     * @param[out] numOfBlocks: suggested number of blocks to be downloaded
     * @param[out] predictedDlTimeForBackupStream predicted download time for
     *             downloading blocks of the backup stream, which has the highest bitrate
     *             among all feasible streams, which bitrate is lower than the select stream's.
     *             The player only uses this output, when streamFeasibility=UNKNOWN_FEASIBILITY;
     * @param[out] predictedRebuffer: predict whether to rebuffer in the future
     * @return: The buffering status at the current buffering level
     */
    virtual BufferingStatus selectStream(
                                    bool    bitrateSwitchFreeze,
                                    int32_t trackId,
                                    double currentPlayoutRate,
                                    AseTimeStamp const& currentPlayoutTime,
                                    std::vector<ITrackBufferInfoPtr> const& trackCurrentInfo,
                                    ullong availableBuffer,
                                    std::vector<ITrackInfoPtr> const& trackFutureInfo,
                                    StreamFeasibility& streamFeasibility,
                                    int32_t& streamId,
                                    int32_t& numOfBlocks,
                                    int32_t& predictedDlTimeForBackupStream,
                                    uint32_t& predictedRebuffer) = 0;

    /**
     * select the stream based only on infor from manifest tracks
     * @param[in] trackId: track id of the selected track
     * @param[in] trackList: manifest track list
     * @param[out] streamIndex: the stream index in the ManifestTrack
     * @param[out] numberOfPredictedRebuffers: number of predicted rebuffers per hour
     */
    virtual void  selectStream(int32_t trackId,
                               std::vector<ManifestTrackPtr> const& trackList,
                               int32_t& streamIndex,
                               uint32_t& numberOfPredictedRebuffers) = 0;

    /**
     * obtain the buffering statistics
     * @param[out] requiredBufferingLength the required buffering length before playback
     * @param[out] estimatedBufferingTime the estimated time for spending on buffering
     */
    virtual void obtainBufferingStats( uint32_t& requiredBufferingLength, uint32_t& estimatedBufferingTime ) = 0;

    /**
     * obtain whether the player is streaming good quality video regards to its throughput
     * @param[out] goodQuality: true if the player is streaming good quality video
     */
    virtual void getStreamingQuality(bool& goodQuality) = 0;

    /**
     * Notify that the playber stop
     */
    virtual void stop() = 0;

    virtual void skip() = 0;

    virtual ~IStreamSelector() {}
};

typedef IStreamSelector::IStreamSelectorPtr IStreamSelectorPtr;

}}

#endif
