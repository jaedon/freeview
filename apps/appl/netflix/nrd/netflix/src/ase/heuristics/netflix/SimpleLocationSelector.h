/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIMPLE_LOCATION_SELECTOR_H
#define SIMPLE_LOCATION_SELECTOR_H

#include <nrdase/DebugLogging.h>
#include <nrdase/IStreamingReporter.h>
#include <nrdase/ILocationSelector.h>
#include <nrdase/Manifest.h>
#include <nrdase/IAseConfigParameter.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdase/IMediaConsumer.h>
#include "IStreamingLocationLevel.h"

#include <nrdbase/Mutex.h>

#include <string>
#include <deque>

namespace netflix {
namespace ase {
DECLARE_LOGGER( LocationSelectorLog );

class ServingStream;
class ServingUrl;
class IStreamingLocationSet;
class INetworkMonitor;

struct ServerSelectionRecord;

class LocationRecord
{
public:
    LocationRecord() : mNetworkFailure(false)
    {
    }

    bool mNetworkFailure;
};

/**
 * @class SimpleLocationSelector
 * @brief The class implementing ILocationSelector
 */
class SimpleLocationSelector : public ILocationSelector
{
public:
    typedef shared_ptr<SimpleLocationSelector> SimpleLocationSelectorPtr;

public:
    static AseErrorCode construct(
            ManifestPtr pManifest,
            IAseConfigParameterPtr pConfigParameter,
            IStreamingReporterPtr pStreamingReporter,
            shared_ptr<INetworkMonitor> pNetworkMonitor,
            SimpleLocationSelectorPtr& pLocationSelector);


    ~SimpleLocationSelector();

    /** ----------------------------------------------------------------------------- */
    /**                     ILocationSelector Interface                               */
    /** ----------------------------------------------------------------------------- */
    // actions

    virtual void configure( IAseConfigParameterPtr pConfig );

    virtual AseErrorCode getPreferredLocationSet(std::string const& streamKey,
                                                 LocationSelectionRequestSource requestSource,
                                                 shared_ptr<IStreamingLocationSet>& pStreamingLocationSet );

    // update ILocationSelector
    virtual void addManifest(ManifestPtr pManifest);
    virtual void removeManifest(ManifestPtr pManifest);
    virtual void setFastplay(bool fastplay);
    virtual void notifyMediaConsumerState(
                        IMediaConsumer::State state );
    virtual void notifyMediaPlaybackState(
                            uint32_t playableLength,
                            bool goodQuality );
    // query state
    virtual bool locationSelectionComplete() const;
    virtual bool bitrateSwitchFreeze() const;

    virtual void networkFailing(    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived );

    void reportStreamingServerFailure(uint32_t cdnId);

    void mediaConsumerState( IMediaConsumer::State& previousState,
                             IMediaConsumer::State& currentState);

    uint32_t locationSelectionSeqNo() const;
    std::string const& selectedLocationKey() const;
    shared_ptr<LocationRecord> obtainLocationRecord(std::string const& locationKey);
    void setLocationRecord(std::string const& locationKey,
                           shared_ptr<LocationRecord> locationRecord);

    IMediaConsumer::State getMediaConsumerState() const;
    uint32_t getMediaPlayableLength() const;

    void reportLocationFailure(std::string const& streamKey, std::string const& locationKey, IStreamingLocationLevel::STATUS);
    // every location for a given bitrate fail
    void reportLocationFailure(std::string const& streamKey);
    void notifyLocationSelected(
                           std::string const& selLocationKey,
                           uint32_t selcdnid,
                           std::string const& selcdnname,
                           uint32_t selLocationRtt,
                           uint32_t selLocationBw,
                           std::string const& testreason,
                           std::string const& selreason,
                           uint32_t fastselthreshold );
    void notifyLocationSelected();

    void updateLocationProbing(bool probing);

    void serverSelected( IStreamingReporter::ServerSelectionReason  reason );

protected:
     SimpleLocationSelector(
            IAseConfigParameterPtr pConfigParameter,
            IStreamingReporterPtr pStreamingReporter,
            shared_ptr<INetworkMonitor> pNetworkMonitor);

    void getConfiguration();

     /** obtain and validate the configured parameter. */
    uint32_t obtainParameter(
                    const std::string& key,
                    uint32_t minVal,
                    uint32_t maxVal,
                    uint32_t defaultVal );

    void updateServingStreams(ManifestPtr pManifest);
    void removeServingStreams(ManifestPtr pManifest);

    bool lowGradeMode() const;

    IStreamingReporter::ServerSelectionReason getServerSelReason(shared_ptr<IStreamingLocationSet> pStreamingLocationSet,
                                                                 shared_ptr<IStreamingLocationSet> pPrevStreamingLocationSet);

    /**
     * check whether to re-run the server selecton logic for the given streamKey
     * @param[in] streamKey
     * @param[out] pServerSelRecord: the corresponding ServerSelectionRecord if any
     * @return: NULL if requesting to re-run the logic, the selected location set otherwise
     */
    shared_ptr<IStreamingLocationSet> checkServerSelection(std::string const& streamKey,
                                                           shared_ptr<ServerSelectionRecord>& pServerSelRecord,
                                                           LocationSelectionRequestSource requestSource);

    // check whether streaming has been failed
    AseErrorCode streamingHealthCheck(shared_ptr<ServingStream> pServingStream);

    void reportStreamingFailureReasons(shared_ptr<ServingStream> pServingStream);
    void reportTempNetworkFailure(AseUrl::HostPort const& destination);

    void printoutServerRecords();

protected:
    IAseConfigParameterPtr mAseConfigParameterPtr;
    shared_ptr<INetworkMonitor> mNetworkMonitorPtr;
    IStreamingReporterPtr mStreamingReporterPtr;

    /* a map storing <stream key, ServingStream> */
    std::map<std::string, shared_ptr<ServingStream> > mServingStreams;

    //The flag that indicates whether the initial location selection is completed
    bool mLocationSelectionFlag;
    bool mRecentConsumerStateChange;

    bool mLowGradeMode; // whether in low grade mode
    bool mLocationProbing; // whether probing a location's throughput
    //Previous media consumer state.
    IMediaConsumer::State mPreviousMediaConsumerState;
    //Current media consumer state.
    IMediaConsumer::State mCurrentMediaConsumerState;

    //Media playable length, in ms.
    uint32_t mMediaPlayableLength;
    //The flag indicates if the Media quality is good.
    bool mMediaGoodQualityFlag;

    // config parameters
    uint32_t mLowGradeModeEnterThreshold;           // seconds
    uint32_t mLowGradeModeExitThreshold;            // seconds

    // maximum number of total network errors during buffering/rebuffering
    uint32_t mMaxNetworkErrorsDuringBuffering;
    // maximum buffering time when the total # of errors reaching maxNetworkErrorsDuringBuffering
    uint32_t mMaxBufferingTimeAllowedWithNetworkError;

    uint32_t mThroughputMeasureWindow;

    uint32_t mLocationSelectionSeqNo;
    uint32_t mPrevLocationSelectionSeqNo;
    std::map<std::string, shared_ptr<LocationRecord> > mLocationRecords;

    uint32_t    mPerformanceCheckInterval;
    std::string mCurrentVideoStreamKey;
    std::string mCurrentAudioStreamKey;
    // a map between stream key and server selection record
    std::map<std::string, shared_ptr<ServerSelectionRecord> > mServerSelectionRecordMap;
    IStreamingReporter::ServerSelectionReason mRecentServerSelReason;

    // The maximum duration the location selector waits before declaring location selection
    // failure after it enters buffering and receives consecutive network failures
    uint32_t    mMaxDomainFailureWaitDuration;
    // The maximum number of consecutive failures before delaring location selection failure
    uint32_t    mMaxConsecutiveFailures;
    // wait to wait for all locations to fail before declaring streaming failure
    bool        mExhaustAllLocationsForFailure;

    bool        mFailedFlag;

    shared_ptr<IStreamingLocationSet> mPrevStreamingLocationSet;
    uint32_t    mPrevSelCdnId;

    std::deque<shared_ptr<Manifest> > mPendingManifests;
    std::deque<shared_ptr<Manifest> > mPendingRemoveManifests;

    AseErrorCode    mFailureCode;

    bool            mHasLowGradeServers;

    std::string     mSelLocationKey;

    AseTimeVal  mNetworkErrorStartTime;
    AseTimeVal  mLastUrlFailoverTime;
    AseErrorCode mLatestNetworkErrorCode;
    uint32_t    mLocationFailoverThreshold;

    mutable Mutex mCmdMutex;
};

struct ServerSelectionRecord
{
    ServerSelectionRecord(shared_ptr<IStreamingLocationSet> selLocationSet,
                          uint32_t selCdnId);

    // selected location set
    shared_ptr<IStreamingLocationSet> mSelLocationSet;
    // selected server/cdn id
    uint32_t    mSelCdnId;
    // the time when the selection happens
    AseTimeVal mSelTime;
};


typedef SimpleLocationSelector::SimpleLocationSelectorPtr SimpleLocationSelectorPtr;

inline IMediaConsumer::State SimpleLocationSelector::getMediaConsumerState() const
{
    return mCurrentMediaConsumerState;
}

inline uint32_t SimpleLocationSelector::getMediaPlayableLength() const
{
    return mMediaPlayableLength;
}

inline
ServerSelectionRecord::ServerSelectionRecord(shared_ptr<IStreamingLocationSet> selLocationSet,
                                             uint32_t selCdnId) :
                            mSelLocationSet(selLocationSet),
                            mSelCdnId(selCdnId),
                            mSelTime(AseTimeVal::now())

{

}

}}
#endif
