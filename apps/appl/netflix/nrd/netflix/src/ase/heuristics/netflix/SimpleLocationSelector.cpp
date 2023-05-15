/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SimpleLocationSelector.h"

#include <nrdase/IMediaConsumer.h>

#include "INetworkMonitor.h"
#include "IStreamingLocationSet.h"
#include "ServingStream.h"
#include "ServingUrl.h"
#include "AseSimpleJsonArraySerializer.h"

namespace netflix {
namespace ase {
DEFINE_LOGGER( LocationSelectorLog );
}}

using namespace netflix;
using namespace ase;
using namespace std;

AseErrorCode SimpleLocationSelector::construct(
            ManifestPtr pManifest,
            IAseConfigParameterPtr pConfigParameter,
            IStreamingReporterPtr pStreamingReporter,
            shared_ptr<INetworkMonitor> pNetworkMonitor,
            SimpleLocationSelectorPtr& pLocationSelector)
{
    pLocationSelector.reset(new SimpleLocationSelector(
                                pConfigParameter,
                                pStreamingReporter,
                                pNetworkMonitor));

    // build mServingStreams using the pManifest and pNetworkMonitor
    pLocationSelector->updateServingStreams(pManifest);
    return AS_NO_ERROR;
}

SimpleLocationSelector::SimpleLocationSelector(
                                    IAseConfigParameterPtr pConfigParameter,
                                    IStreamingReporterPtr pStreamingReporter,
                                    shared_ptr<INetworkMonitor> pNetworkMonitor)
                        : mAseConfigParameterPtr(pConfigParameter),
                          mNetworkMonitorPtr(pNetworkMonitor),
                          mStreamingReporterPtr(pStreamingReporter),
                          mLocationSelectionFlag( false ),
                          mRecentConsumerStateChange( false ),
                          mLowGradeMode(false),
                          mLocationProbing(false),
                          mPreviousMediaConsumerState( IMediaConsumer::MEDIA_CONSUMER_STOPPED_STATE ),
                          mCurrentMediaConsumerState( IMediaConsumer::MEDIA_CONSUMER_STOPPED_STATE ),
                          mMediaPlayableLength( 0 ),
                          mMediaGoodQualityFlag( false ),
                          mLocationSelectionSeqNo(0),
                          mPrevLocationSelectionSeqNo(0),
                          mPerformanceCheckInterval(500),
                          mCurrentVideoStreamKey(""),
                          mCurrentAudioStreamKey(""),
                          mRecentServerSelReason(IStreamingReporter::SERVER_UNKNOWN),
                          mFailedFlag(false),
                          mPrevStreamingLocationSet(shared_ptr<IStreamingLocationSet>()),
                          mPrevSelCdnId(0),
                          mFailureCode(AS_NO_ERROR),
                          mHasLowGradeServers(false),
                          mNetworkErrorStartTime(AseTimeVal::INFINITE),
                          mLastUrlFailoverTime(AseTimeVal::INFINITE),
                          mLatestNetworkErrorCode(AS_NO_ERROR),
                          mLocationFailoverThreshold(0),
                          mCmdMutex(LOCSEL_CMD_MUTEX, "LocSelCmdMutex")

{
    mServingStreams.clear();

    getConfiguration();
}

void SimpleLocationSelector::configure( IAseConfigParameterPtr pConfig )
{
    mAseConfigParameterPtr = pConfig;

    getConfiguration();
}

void SimpleLocationSelector::getConfiguration()
{
    mLowGradeModeEnterThreshold = obtainParameter(
                                            "lowGradeModeEnterThreshold",
                                            10 * 1000,
                                            3600 * 1000 * 10,
                                            120 * 1000 );

    mLowGradeModeExitThreshold = obtainParameter(
                                            "lowGradeModeExitThreshold",
                                            10 * 1000,
                                            3600 * 1000 * 10,
                                            90 * 1000 );

    mMaxDomainFailureWaitDuration = obtainParameter(
                                            "maxDomainFailureWaitDuration",
                                            1000,
                                            3600000,
                                            30000);

    mThroughputMeasureWindow =
        mAseConfigParameterPtr->getParameterAs<uint32_t>("throughputMeasureWindow", DEFAULT_THROUGHPUT_MEASURE_WINDOW);

    //Make sure the enter threshold is higher than exit threshold,
    //otherwise set both to default
    if (mLowGradeModeEnterThreshold <= mLowGradeModeExitThreshold)
    {
        mLowGradeModeEnterThreshold = 45 * 1000;
        mLowGradeModeExitThreshold = 30 * 1000;
    }

    mLocationFailoverThreshold =
        mAseConfigParameterPtr->getParameterAs<uint32_t>("locationFailoverThreshold", 8000);
}

SimpleLocationSelector::~SimpleLocationSelector()
{
    DEBUGLOG(LocationSelectorLog, "SimpleLocationSelector desctructor");
}

AseErrorCode SimpleLocationSelector::getPreferredLocationSet(std::string const& streamKey,
        LocationSelectionRequestSource requestSource,
        IStreamingLocationSetPtr& pStreamingLocationSet )
{
    ManifestPtr pManifest;
    {
        ScopedMutex scopedMutexResource(mCmdMutex);
        while ( !mPendingManifests.empty() )
        {
            pManifest = mPendingManifests.front();
            updateServingStreams(pManifest);

            mNetworkMonitorPtr->addServingLocationsSync(pManifest->getServingLocations());

            mPendingManifests.pop_front();
        }

        while ( !mPendingRemoveManifests.empty() )
        {
            pManifest = mPendingRemoveManifests.front();
            removeServingStreams(pManifest);
            mPendingRemoveManifests.pop_front();
        }
    }

    AseErrorCode retCode = AS_NO_ERROR;

    if ( mFailedFlag )
    {
        return mFailureCode;
    }

    if ( mServingStreams.find(streamKey) == mServingStreams.end() )
    {
        DEBUGLOG(
                LocationSelectorLog,
                "getPreferredLocationSet: can not found %s", streamKey.c_str());
        mFailedFlag = true;
        mFailureCode = AS_DOMAIN_SELECTION_FAILURE_BAD_STREAM;
        return mFailureCode;
    }

    ServingStreamPtr pServingStream = mServingStreams[streamKey];
    MediaType mediaType = pServingStream->mediaType();

    if ( streamingHealthCheck(pServingStream) != AS_NO_ERROR )
    {
        return mFailureCode;
    }


    // check whether we have already had valid selected location set for this stream
    shared_ptr<ServerSelectionRecord> pPrevServerSelRecord;
    shared_ptr<ServerSelectionRecord> pServerSelRecord;
    pStreamingLocationSet = checkServerSelection(streamKey, pPrevServerSelRecord, requestSource);

    if ( (pStreamingLocationSet.get()) && !mRecentConsumerStateChange)
    {
        if ( (requestSource == VIDEO_DOWNLOADING) || (requestSource == AUDIO_DOWNLOADING) )
        {
            uint32_t selCdnId = pStreamingLocationSet->getPrimaryCdnId();
            IStreamingReporter::ServerSelectionReason selReason = IStreamingReporter::SERVER_UNKNOWN;
            pServerSelRecord.reset(new ServerSelectionRecord(pStreamingLocationSet, pStreamingLocationSet->getPrimaryCdnId()) );
            pServerSelRecord->mSelTime = pPrevServerSelRecord->mSelTime;
            mServerSelectionRecordMap[streamKey] = pServerSelRecord;

            if ( mediaType == MEDIA_VIDEO )
            {
                // only log serversel for video
                if ( (mPrevStreamingLocationSet.get())
                        && (pStreamingLocationSet->getLocationSetKey() == mPrevStreamingLocationSet->getLocationSetKey()) )
                {
                    // the same location set
                    if ( mPrevSelCdnId != selCdnId )
                    {
                        selReason = IStreamingReporter::SERVER_SERVERFAILOVER;
                        mStreamingReporterPtr->serverSwitch(selCdnId, pStreamingLocationSet->getLocationSetMonitor(),
                                                            selReason,
                                                            pServingStream->manifestStream()->getNominalBitrate());
                        DEBUGLOG(LocationSelectorLog, "serverSwitch: selCdnId: %u, reason: %u, stream key: %s",
                                                        selCdnId, selReason,
                                                        streamKey.c_str());
                    }
                }
                else if ( mPrevSelCdnId != selCdnId )
                {
                    selReason = IStreamingReporter::SERVER_BITRATE;
                    mStreamingReporterPtr->serverSwitch(selCdnId, pStreamingLocationSet->getLocationSetMonitor(),
                                                        selReason,
                                                        pServingStream->manifestStream()->getNominalBitrate());

                    DEBUGLOG(LocationSelectorLog, "serverSwitch: selCdnId: %u, reason: %u, stream key: %s",
                                                   selCdnId, selReason,
                                                   streamKey.c_str());
                }

                mPrevStreamingLocationSet = pStreamingLocationSet;
                mPrevSelCdnId = mPrevStreamingLocationSet->getPrimaryCdnId();
                mNetworkMonitorPtr->switchLocationSet( pStreamingLocationSet->getLocationSetMonitor() );
            }
        }

        // update stream keys
        if ( requestSource == VIDEO_DOWNLOADING )
        {
            mCurrentVideoStreamKey = streamKey;
        }
        else if ( requestSource == AUDIO_DOWNLOADING )
        {
            mCurrentAudioStreamKey = streamKey;
        }

        return AS_NO_ERROR;
    }

    mRecentConsumerStateChange = false;

    mPrevLocationSelectionSeqNo = mLocationSelectionSeqNo;

    // update the server selection
    retCode = pServingStream->getPreferredLocationSet(pStreamingLocationSet, requestSource);

    if ( retCode == AS_NO_ERROR )
    {
        if ( (requestSource == VIDEO_DOWNLOADING) || (requestSource == AUDIO_DOWNLOADING) )
        {
            shared_ptr<ILocationSetMonitor> pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();
            mNetworkMonitorPtr->switchLocationSet( pLocationSetMonitor );
            // insert a new server selection record
            pServerSelRecord.reset(new ServerSelectionRecord(pStreamingLocationSet, pStreamingLocationSet->getPrimaryCdnId()) );
            mServerSelectionRecordMap[streamKey] = pServerSelRecord;

            bool changeStream = false;
            if ( mediaType == MEDIA_VIDEO )
            {
                changeStream = !(mCurrentVideoStreamKey == streamKey);
                mCurrentVideoStreamKey = streamKey;
            }
            else if ( mediaType == MEDIA_AUDIO )
            {
                changeStream = !(mCurrentAudioStreamKey == streamKey);
                mCurrentAudioStreamKey = streamKey;
            }

            if ( mediaType == MEDIA_VIDEO )
            {
                uint32_t selCdnId = pStreamingLocationSet->getPrimaryCdnId();
                if ( (mPrevStreamingLocationSet.get()) &&
                        (pStreamingLocationSet->getLocationSetKey() == mPrevStreamingLocationSet->getLocationSetKey()) )
                {
                    // The same location set
                    if ( mPrevSelCdnId != selCdnId )
                    {
                        mStreamingReporterPtr->serverSwitch(selCdnId, pLocationSetMonitor, mRecentServerSelReason,
                                                        pServingStream->manifestStream()->getNominalBitrate());
                        DEBUGLOG( LocationSelectorLog, "serverSwitch: selCdnId: %u, reason: %u, stream key: %s",
                                                        selCdnId, mRecentServerSelReason, streamKey.c_str());
                    }
                }
                else if ( mPrevSelCdnId != selCdnId )
                {
                    if ( changeStream )
                    {
                        mStreamingReporterPtr->serverSwitch(selCdnId, pLocationSetMonitor, IStreamingReporter::SERVER_BITRATE,
                                pServingStream->manifestStream()->getNominalBitrate());
                        DEBUGLOG( LocationSelectorLog, "serverSwitch: selCdnId: %u, reason: %u, stream key: %s",
                                selCdnId, IStreamingReporter::SERVER_BITRATE, streamKey.c_str());
                    }
                    else
                    {
                        IStreamingReporter::ServerSelectionReason selReason =
                            getServerSelReason(pStreamingLocationSet, mPrevStreamingLocationSet);
                        if ( selReason == IStreamingReporter::SERVER_UNKNOWN )
                            selReason = mRecentServerSelReason;

                        mStreamingReporterPtr->serverSwitch(selCdnId, pLocationSetMonitor, selReason,
                                pServingStream->manifestStream()->getNominalBitrate());
                        DEBUGLOG( LocationSelectorLog, "serverSwitch: selCdnId: %u, reason: %u, stream key: %s",
                                selCdnId, selReason, streamKey.c_str());
                    }
                }
            }

            mPrevStreamingLocationSet = pStreamingLocationSet;
            mPrevSelCdnId = mPrevStreamingLocationSet->getPrimaryCdnId();
            retCode = AS_NO_ERROR;
            DEBUGLOG( LocationSelectorLog,
                                  "getPreferredLocationSet: stream key: %s, url: %s",
                                  streamKey.c_str(),
                                  pStreamingLocationSet->getDownloadableUrl()->getUrl().GetPrefix().c_str());
        }

    }
    else
    {
        reportStreamingFailureReasons(pServingStream);
    }

    return retCode;
}

void SimpleLocationSelector::addManifest(ManifestPtr pManifest)
{
    ScopedMutex scopedMutexResource(mCmdMutex);
    mPendingManifests.push_back(pManifest);
}

void SimpleLocationSelector::removeManifest(ManifestPtr pManifest)
{
    ScopedMutex scopedMutexResource(mCmdMutex);
    mPendingRemoveManifests.push_back(pManifest);
}

void SimpleLocationSelector::setFastplay(bool fastplay)
{
    for ( map<string, ServingStreamPtr>::iterator iter = mServingStreams.begin();
            iter != mServingStreams.end();
            ++iter )
    {
        (*iter).second->setFastplay(fastplay);
    }
}

void SimpleLocationSelector::notifyMediaConsumerState(
                        IMediaConsumer::State state )
{
    if (mCurrentMediaConsumerState != state)
    {
        mPreviousMediaConsumerState = mCurrentMediaConsumerState;
        mCurrentMediaConsumerState = state;
        mRecentConsumerStateChange = true;
        for ( map<string, ServingStreamPtr>::iterator iter = mServingStreams.begin();
                iter != mServingStreams.end();
                ++iter )
        {
            (*iter).second->consumerStateChange();
        }

        if ( (mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_BUFFERING_STATE) ||
                (mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_REBUFFERING_STATE) )
        {
            // kick off another location selection process
            mLocationSelectionFlag = false;
            if (mLatestNetworkErrorCode != AS_NO_ERROR)
                mNetworkErrorStartTime = AseTimeVal::now();

            mNetworkMonitorPtr->purge();
        }

        DEBUGLOG(
                LocationSelectorLog,
                "LocationSelector was notified the media consumer state change, old state: %s, new state: %s.",
                IMediaConsumer::getStateName( mPreviousMediaConsumerState ),
                IMediaConsumer::getStateName( mCurrentMediaConsumerState ));
    }
    return;
}

void SimpleLocationSelector::mediaConsumerState(
        IMediaConsumer::State& previousState,
        IMediaConsumer::State& currentState)
{
    previousState = mPreviousMediaConsumerState;
    currentState = mCurrentMediaConsumerState;
}

/**
 * low grade cdn support
 */
void SimpleLocationSelector::notifyMediaPlaybackState(
                                    uint32_t playableLength,
                                    bool goodQuality )
{
    if ( mHasLowGradeServers )
    {
        if ( !lowGradeMode()
                && ((playableLength > mLowGradeModeEnterThreshold) && goodQuality) )
        {
            DEBUGLOG(
                    LocationSelectorLog,
                    "was notified the low grade entering playback state, playable length: %u, good quality: %s.",
                    playableLength,
                    goodQuality ? "True" : "False" );
            for ( map<string, ServingStreamPtr>::iterator iter = mServingStreams.begin();
                    iter != mServingStreams.end();
                    ++iter )
            {
                (*iter).second->enableLowGradeLevel();
            }
            mLowGradeMode = true;
            mServerSelectionRecordMap.clear();
        }

        if ( lowGradeMode()
                && (playableLength <= mLowGradeModeExitThreshold)  )
        {
        DEBUGLOG(
                LocationSelectorLog,
                "was notified the low grade exiting playback state, playable length: %u, good quality: %s.",
                playableLength,
                goodQuality ? "True" : "False" );
        for ( map<string, ServingStreamPtr>::iterator iter = mServingStreams.begin();
                iter != mServingStreams.end();
                ++iter )
        {
            (*iter).second->disableLowGradeLevel();
        }
        mLowGradeMode = false;
        mServerSelectionRecordMap.clear();
        }
    }

    mMediaPlayableLength = playableLength;
    mMediaGoodQualityFlag = goodQuality;

    return;
}


bool SimpleLocationSelector::locationSelectionComplete() const
{
    return mLocationSelectionFlag;
}


bool SimpleLocationSelector::lowGradeMode() const
{
    return mLowGradeMode;
}

/** obtain and validate the configured parameter */
uint32_t SimpleLocationSelector::obtainParameter(
                                    const std::string& key,
                                    uint32_t minVal,
                                    uint32_t maxVal,
                                    uint32_t defaultVal )
{
    uint32_t retVal;

    retVal = mAseConfigParameterPtr->getParameterAs<uint32_t>(
                                                        key,
                                                        defaultVal );
    if ((retVal < minVal) || (retVal > maxVal))
    {
        retVal = defaultVal;
    }

    return retVal;
}

void SimpleLocationSelector::updateServingStreams(ManifestPtr pManifest)
{
    ServingStreamPtr pServingStream;
    vector<ManifestTrackPtr> const& manifestTracks = pManifest->getTracks();
    vector<ManifestServingLocationPtr> const& servingLocations = pManifest->getServingLocations();
    map<uint32_t, ManifestServerPtr> servers = pManifest->getServers();

    for ( map<uint32_t, ManifestServerPtr>::iterator serverIter = servers.begin();
            serverIter != servers.end();
            ++serverIter )
    {
        if ( (serverIter->second)->getLowGradeFlag() )
        {
            mHasLowGradeServers = true;
            break;
        }
    }


#if 0
    DEBUGLOG(LocationSelectorLog, "updateServingStreams: locations: %zu",
                        servingLocations.size());
    for ( vector<ManifestServingLocationPtr>::const_iterator locIter = servingLocations.begin();
            locIter != servingLocations.end();
            ++locIter )
    {
        DEBUGLOG(LocationSelectorLog, "location: %s:", (*locIter)->getLocationKey().c_str());
    }
#endif

    shared_ptr<LocationRecord> pLocationRecord;
    for ( vector<ManifestServingLocationPtr>::const_iterator locationIter = servingLocations.begin();
                locationIter != servingLocations.end();
                ++locationIter )
    {
        string locationKey = (*locationIter)->getLocationKey();
        if ( mLocationRecords.find(locationKey) == mLocationRecords.end() )
        {
            pLocationRecord.reset(new LocationRecord);
            mLocationRecords[locationKey] = pLocationRecord;
        }
    }

    for ( vector<ManifestTrackPtr>::const_iterator trackIter = manifestTracks.begin();
            trackIter != manifestTracks.end();
            ++trackIter )
    {
        vector<ManifestStreamPtr> const& manifestStreams = (*trackIter)->getStreams();
        MediaType mediaType = (*trackIter)->getMediaType();
        if ( (mediaType != MEDIA_AUDIO) && (mediaType != MEDIA_VIDEO) )
            continue;

        for ( vector<ManifestStreamPtr>::const_iterator streamIter = manifestStreams.begin();
                streamIter != manifestStreams.end();
                ++streamIter )
        {
            string streamKey = (*streamIter)->getStreamKey();
            if ( mServingStreams.find(streamKey) == mServingStreams.end() )
            {
                ServingStream::construct( static_pointer_cast<SimpleLocationSelector>(shared_from_this()),
                        (*trackIter)->getMediaType(), *streamIter,
                        servingLocations, servers, mNetworkMonitorPtr,
                        mAseConfigParameterPtr, pServingStream );
                mServingStreams[ streamKey ] = pServingStream;
            }
        }

    }
    return;
}

void SimpleLocationSelector::removeServingStreams(ManifestPtr pManifest)
{
    vector<ManifestTrackPtr> const& manifestTracks = pManifest->getTracks();

    for ( vector<ManifestTrackPtr>::const_iterator trackIter = manifestTracks.begin();
            trackIter != manifestTracks.end();
            ++trackIter )
    {
        vector<ManifestStreamPtr> const& manifestStreams = (*trackIter)->getStreams();
        MediaType mediaType = (*trackIter)->getMediaType();
        if ( (mediaType != MEDIA_AUDIO) && (mediaType != MEDIA_VIDEO) )
            continue;

        for ( vector<ManifestStreamPtr>::const_iterator streamIter = manifestStreams.begin();
                streamIter != manifestStreams.end();
                ++streamIter )
        {
            string streamKey = (*streamIter)->getStreamKey();
            DEBUGLOG(LocationSelectorLog,
                    "remove serving stream: %s", (*streamIter)->getStreamKey().c_str());
            mServingStreams.erase(streamKey);
            mServerSelectionRecordMap.erase(streamKey);
        }
    }
}

uint32_t SimpleLocationSelector::locationSelectionSeqNo() const
{
    return mLocationSelectionSeqNo;
}

std::string const& SimpleLocationSelector::selectedLocationKey() const
{
    return mSelLocationKey;
}

bool SimpleLocationSelector::bitrateSwitchFreeze() const
{
    if ( mLowGradeMode || mLocationProbing )
        return true;
    else
        return false;
}

void SimpleLocationSelector::reportLocationFailure(string const& streamKey, std::string const& locationKey, IStreamingLocationLevel::STATUS status)
{
    ++mLocationSelectionSeqNo;
    DEBUGLOG(LocationSelectorLog,
            "reportLocationFailure: seq no: %u, status: %u",
            mLocationSelectionSeqNo, status);
    if ( status == IStreamingLocationLevel::TEMP_FAILED_STATE )
    {
        shared_ptr<LocationRecord> pLocationRecord;
        pLocationRecord.reset(new LocationRecord());
        pLocationRecord->mNetworkFailure = true;
        mLocationRecords[locationKey] = pLocationRecord;
    }

    mServerSelectionRecordMap.erase(streamKey);
}

void SimpleLocationSelector::reportLocationFailure(string const& streamKey)
{
    NRDP_UNUSED(streamKey);
    DEBUGLOG(LocationSelectorLog,
            "reportAllLocationFailure for stream: %s", streamKey.c_str());
    ++mLocationSelectionSeqNo;
}


shared_ptr<LocationRecord> SimpleLocationSelector::obtainLocationRecord(std::string const& locationKey)
{
    return mLocationRecords[locationKey];
}


void SimpleLocationSelector::setLocationRecord(std::string const& locationKey, shared_ptr<LocationRecord> pLocationRecord)
{
    mLocationRecords[locationKey] = pLocationRecord;
}

void SimpleLocationSelector::notifyLocationSelected()
{
    ++mLocationSelectionSeqNo;
    DEBUGLOG(LocationSelectorLog,
            "notifyLocationSelected from multi location selector");
    mLocationSelectionFlag = true;
}

void SimpleLocationSelector::notifyLocationSelected(
                                      std::string const& selLocationKey,
                                      uint32_t selcdnid,
                                      std::string const& selcdnname,
                                      uint32_t selLocationRtt,
                                      uint32_t selLocationBw,
                                      std::string const& testreason,
                                      std::string const& selreason,
                                      uint32_t fastselthreshold )
{
    ++mLocationSelectionSeqNo;
    DEBUGLOG(LocationSelectorLog,
             "notifyLocationSelected, location: %s, sel cdn id: %u, seq no: %u",
             selLocationKey.c_str(),
             selcdnid, mLocationSelectionSeqNo);
    mSelLocationKey = selLocationKey;
    mLocationSelectionFlag = true;

    std::vector<std::vector<AseSimpleJsonArraySerializer::SimpleObj> > bwdataJsonArray;

    for ( map<string, shared_ptr<LocationRecord> >::iterator recordIter = mLocationRecords.begin();
            recordIter != mLocationRecords.end();
            ++recordIter )
    {
        string locationKey = (*recordIter).first;
        shared_ptr<ILocationSetMonitor> pLocationSetMonitor = mNetworkMonitorPtr->getLocationSetMonitor(locationKey);

        if ( pLocationSetMonitor )
        {
            INetworkMonitor::PredictionSource source;
            double confidence;
            uint32_t throughput = mNetworkMonitorPtr->getPredictedThroughput(pLocationSetMonitor, mThroughputMeasureWindow, source,confidence);
            uint32_t avgConnectTime, varConnectTime, numberOfConnectionFailures;
            pLocationSetMonitor->connectionStat(avgConnectTime, varConnectTime, numberOfConnectionFailures);
            string ipAddr = pLocationSetMonitor->getIpAddr();

            if ( (throughput > 0) && (ipAddr != "") )
            {
                // still uses the old terms
                std::vector<AseSimpleJsonArraySerializer::SimpleObj> bwdataObjArray;
                bwdataObjArray.push_back(AseSimpleJsonArraySerializer::SimpleObj("id", locationKey));
                bwdataObjArray.push_back(AseSimpleJsonArraySerializer::SimpleObj("locid", locationKey));
                bwdataObjArray.push_back(AseSimpleJsonArraySerializer::SimpleObj("ip", ipAddr));
                bwdataObjArray.push_back(AseSimpleJsonArraySerializer::SimpleObj("rtt", avgConnectTime));
                bwdataObjArray.push_back(AseSimpleJsonArraySerializer::SimpleObj("bw", throughput));
                bwdataJsonArray.push_back(bwdataObjArray);
            }
        }
    }
    std::string locationBwData = AseSimpleJsonArraySerializer::toJsonString(bwdataJsonArray);

    mStreamingReporterPtr->serverSelected( selcdnid, selcdnname,
                                           selLocationRtt, selLocationBw,
                                           locationBwData, testreason, selreason,
                                           fastselthreshold);

    mStreamingReporterPtr->reportKeyStartplayEvent("domainselected");

}

void SimpleLocationSelector::updateLocationProbing(bool probing)
{
    mLocationProbing = probing;
}

void SimpleLocationSelector::networkFailing(    AseErrorCode            latestStatus,
                                                AseUrl::HostPort const& destination,
                                                AseTimeVal const*       pLatestErrorTime,
                                                AseTimeVal const&       latestDataReceived )
{
    NRDP_UNUSED(pLatestErrorTime);
    AseTimeVal now = AseTimeVal::now();
    DEBUGLOG(LocationSelectorLog, "networkFailing: %d, %s, %lldms ago, %llums ago ",
                latestStatus, destination.first.c_str(),
                pLatestErrorTime ? (now - *pLatestErrorTime).ms() : -1,
                (now - latestDataReceived).ms());
    do
    {
        if ( latestStatus == AS_NO_ERROR )
        {
            mLastUrlFailoverTime = AseTimeVal::INFINITE;
            mNetworkErrorStartTime = AseTimeVal::INFINITE;
            mLatestNetworkErrorCode = AS_NO_ERROR;
            break;
        }

        if ( mLastUrlFailoverTime == AseTimeVal::INFINITE )
        {
            // first error received
            mLastUrlFailoverTime = latestDataReceived;
            mLatestNetworkErrorCode = latestStatus;
            if ( (mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_BUFFERING_STATE) ||
                    (mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_REBUFFERING_STATE) )
            {
                mNetworkErrorStartTime = now;
            }
            break;
        }

        if ( AseTimeVal::fromMS(mLocationFailoverThreshold) + mLastUrlFailoverTime < now )
        {
            // This location has failed for some time, so make it temporarily failed.
            reportTempNetworkFailure(destination);
            mLastUrlFailoverTime = now;
            break;
        }

    } while (false);
}

void SimpleLocationSelector::reportTempNetworkFailure(AseUrl::HostPort const& destination)
{
    for ( map<string, ServingStreamPtr>::iterator iter = mServingStreams.begin();
            iter != mServingStreams.end();
            ++iter )
        (*iter).second->reportTempNetworkFailure(destination);
    return;
}

void SimpleLocationSelector::reportStreamingServerFailure(uint32_t cdnId)
{
    mStreamingReporterPtr->reportStreamingServerFailure(cdnId);
}

void SimpleLocationSelector::serverSelected( IStreamingReporter::ServerSelectionReason reason )
{
    mRecentServerSelReason = reason;
}

shared_ptr<IStreamingLocationSet>
SimpleLocationSelector::checkServerSelection(std::string const& streamKey,
        shared_ptr<ServerSelectionRecord>& pServerSelRecord,
        LocationSelectionRequestSource requestSource)
{
    NRDP_UNUSED(requestSource);
    shared_ptr<IStreamingLocationSet> pStreamingLocationSet = shared_ptr<IStreamingLocationSet>();
    pServerSelRecord = shared_ptr<ServerSelectionRecord>();
    do
    {
        if ( mServerSelectionRecordMap.find(streamKey) == mServerSelectionRecordMap.end() )
        {
            DEBUGLOG(LocationSelectorLog, "first selection for stream: %s", streamKey.c_str());
            // have not selected for this stream key yet
            break;
        }
        pServerSelRecord = mServerSelectionRecordMap[streamKey];

        if ( mPrevLocationSelectionSeqNo != mLocationSelectionSeqNo )
        {
            // have some new location selecton result
            break;
        }

        AseTimeVal now = AseTimeVal::now();
        if ( now > pServerSelRecord->mSelTime + AseTimeVal::fromMS(mPerformanceCheckInterval) )
        {
            // pass the performance check time
            break;
        }

        pStreamingLocationSet = pServerSelRecord->mSelLocationSet;
    } while (false);

#if 0
    printoutServerRecords();
#endif

    return pStreamingLocationSet;
}

void
SimpleLocationSelector::printoutServerRecords()
{
    DEBUGLOG(LocationSelectorLog, "printoutServerRecords:");
    for ( map<string, shared_ptr<ServerSelectionRecord> >::iterator iter = mServerSelectionRecordMap.begin();
            iter != mServerSelectionRecordMap.end();
            ++iter )
    {
        DEBUGLOG(LocationSelectorLog, "stream: %s, location: %s",
                iter->first.c_str(),
                iter->second->mSelLocationSet->getLocationSetKey().c_str());
    }

}

AseErrorCode
SimpleLocationSelector::streamingHealthCheck(ServingStreamPtr pServingStream)
{
    if ( (mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_BUFFERING_STATE) ||
                (mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_REBUFFERING_STATE) )
    {
        AseTimeVal now = AseTimeVal::now();
        // decide whether to fail the location selection process
        uint32_t failureWaitDuration = mMaxDomainFailureWaitDuration;
        if ( mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_BUFFERING_STATE )
            failureWaitDuration *= 2;

        // Failure metric: The playback will be failured, if the player has experienced consecutive
        // network errors for longer than failureWaitDuration during the buffering
        if ( now > mNetworkErrorStartTime + AseTimeVal::fromMS(failureWaitDuration) )
        {
            mFailedFlag = true;
            DEBUGLOG(LocationSelectorLog,
                    "Having failed long time for to declare location selection failure");
            mFailureCode = AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_LONG;
            reportStreamingFailureReasons(pServingStream);
            return mFailureCode;
        }
    }
    return AS_NO_ERROR;
}

    void
SimpleLocationSelector::reportStreamingFailureReasons(ServingStreamPtr pServingStream)
{
    AseErrorCode networkErrorCode;
    int32_t httpStatusCode;
    pServingStream->obtainErrorReason(networkErrorCode, httpStatusCode);
    DEBUGLOG(LocationSelectorLog, "reportStreamingFailureReasons: network error code: %d, http status code: %d",
                        mLatestNetworkErrorCode, httpStatusCode);
    mStreamingReporterPtr->streamingFailureReasons(mLatestNetworkErrorCode, httpStatusCode);
    return;
}

IStreamingReporter::ServerSelectionReason
SimpleLocationSelector::getServerSelReason(shared_ptr<IStreamingLocationSet> pStreamingLocationSet,
                                           shared_ptr<IStreamingLocationSet> pPrevStreamingLocationSet)
{
    IStreamingReporter::ServerSelectionReason reason = IStreamingReporter::SERVER_UNKNOWN;
    if ( (pStreamingLocationSet.get() == NULL) || (pPrevStreamingLocationSet.get() == NULL) )
        return reason;

    bool lowGradeFlag1 = pStreamingLocationSet->lowGrade();
    bool lowGradeFlag2 = pPrevStreamingLocationSet->lowGrade();

    if ( !lowGradeFlag2 && lowGradeFlag1 )
    {
        reason = IStreamingReporter::SERVER_TOLOWGRADE;
    }
    else if ( lowGradeFlag2 && !lowGradeFlag1 )
    {
        reason = IStreamingReporter::SERVER_FROMLOWGRADE;
    }

    return reason;
}

