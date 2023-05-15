/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "OrigStreamingLocationLevel.h"

#include "SimpleLocationSelector.h"
#include "INetworkMonitor.h"
#include "SingleStreamingLocationSet.h"
#include "ServingStream.h"

using namespace netflix::ase;
using namespace std;

OrigStreamingLocationLevel::OrigStreamingLocationLevel(
                                shared_ptr<SimpleLocationSelector> pLocationSelector,
                                shared_ptr<ServingStream> pServingStream,
                                ManifestUrls const& manifestUrls,
                                std::set<ManifestServingLocationPtr, DereferenceLess> servingLocations,
                                INetworkMonitorPtr pNetworkMonitor,
                                IAseConfigParameterPtr pConfigParameter,
                                uint32_t level, bool lowGradeFlag) :
                                        StreamingLocationLevel(pLocationSelector, pServingStream, pConfigParameter,
                                                               level, lowGradeFlag),
                                        mLocationSelectionSeqNo(0)
{
    initializeLocationLevel(manifestUrls, servingLocations, pNetworkMonitor);

    mFastDomainSelectionBwThreshold = mConfigParameterPtr->getParameterAs<uint32_t>(
                            "fastDomainSelectionBwThreshold",
                            100,
                            1000000,
                            2000 );
}


AseErrorCode
OrigStreamingLocationLevel::getPreferredLocationSet(IStreamingLocationSetPtr& pStreamingLocationSet,
                                                    ILocationSelector::LocationSelectionRequestSource /*requestSource*/)
{
    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if ( pLocationSelector == SimpleLocationSelectorPtr() )
        return AS_NO_ERROR;

    bool locationSelected = pLocationSelector->locationSelectionComplete();
    uint32_t seqNo = pLocationSelector->locationSelectionSeqNo();
    bool forceSelection = false;
    bool enableLogging = false;
    IMediaConsumer::State previousState, currentState;
    pLocationSelector->mediaConsumerState(previousState, currentState);
    DEBUGLOG(LocationSelectorLog,
                    "OrigStreamingLocationLevel::getPreferredLocationSet, level: %u "
                    "locationSelected: %u, seqNo: %u:%u, stored MediaConsumer State: %s:%s, "
                    "new MediaConsumer State: %s:%s",
                    mLevel,
                    locationSelected, seqNo, mLocationSelectionSeqNo,
                    IMediaConsumer::getStateName( mPreviousMediaConsumerState ),
                    IMediaConsumer::getStateName( mCurrentMediaConsumerState ),
                    IMediaConsumer::getStateName( previousState ),
                    IMediaConsumer::getStateName( currentState ) );

    if ( locationSelected )
    {
        if ( (mLocationSelectionSeqNo < seqNo) ||
                (mConsumerStateChange && (currentState == IMediaConsumer::MEDIA_CONSUMER_PLAYING_STATE)) )
        {
            // location has been selected for other bitrates or a location is failed
            enableLogging = false;
            forceSelection = true;
            updateLocationLevel(forceSelection, enableLogging);
            mLocationSelectionSeqNo = seqNo;
        }
    }
    else
    {
        forceSelection = false;
        if (mStatus == INIT_STATE)
        {
            enableLogging = true;
            if ( seqNo == 0 )
            {
                mTestReason = TEST_STARTUP;
                if ( mFastplay )
                    forceSelection = true;
            }
            else
            {
                mTestReason = TEST_SEEK;
            }
            updateLocationLevel(forceSelection, enableLogging);
        }
        else
        {
            if ( mConsumerStateChange )
            {
                // media consumer state change
                switch (currentState)
                {
                    case IMediaConsumer::MEDIA_CONSUMER_BUFFERING_STATE:
                        if ( seqNo > 0 )
                        {
                            mTestReason = TEST_SEEK;
                            mSelReason = SEL_REUSE_PREVIOUS;
                            notifyLocationSelected(mCurrentLocationSetIndex, mFastDomainSelectionBwThreshold);
                        }
                        else
                        {
                            mTestReason = TEST_STARTUP;
                            mStatus = MEASURING_STATE;
                        }
                        break;
                    case IMediaConsumer::MEDIA_CONSUMER_REBUFFERING_STATE:
                        mStatus = MEASURING_STATE;
                        mTestReason = TEST_REBUFFER;
                        break;
                    case IMediaConsumer::MEDIA_CONSUMER_PLAYING_STATE:
                        break;
                    default:
                        break;
                }

            }

            if ( mStatus == MEASURING_STATE )
            {
                forceSelection = false;
                enableLogging = true;
                updateLocationLevel(forceSelection, enableLogging);
            }
        }
    }


    DEBUGLOG(LocationSelectorLog, "status: %u, current location set index: %u, key: %s",
            mStatus, mCurrentLocationSetIndex,
            mStreamingLocationSetList[mCurrentLocationSetIndex]->getLocationSetKey().c_str());

    mPreviousMediaConsumerState = previousState;
    mCurrentMediaConsumerState = currentState;
    mConsumerStateChange = false;
    pStreamingLocationSet = mStreamingLocationSetList[mCurrentLocationSetIndex];
    checkNewServerSelected(pStreamingLocationSet);
    return AS_NO_ERROR;
}


/** unprocessed networking events only trigger changes from state
 * MEASURING_STATE and SELECTED_STATE
 */
IStreamingLocationLevel::STATUS OrigStreamingLocationLevel::getStatus()
{
    uint32_t indexForNextValidLocationSet = locationLevelHealthCheck();
#if 0
    DEBUGLOG(LocationSelectorLog,
            "indexes: %u, %u",
            indexForNextValidLocationSet, mCurrentLocationSetIndex);
#endif
    if ( indexForNextValidLocationSet != mCurrentLocationSetIndex )
    {
        mSelReason = SEL_NEXT_DOMAIN;
        mTestReason = TEST_DLFAIL;
        mCurrentLocationSetIndex = indexForNextValidLocationSet;
        mRecentServerSelReason = IStreamingReporter::SERVER_LOCATIONFAILOVER;
        DEBUGLOG(LocationSelectorLog,
                 "OrigStreamingLocationLevel::getStatus, current location set index: %u",
                 mCurrentLocationSetIndex);
        notifyLocationSelected(mCurrentLocationSetIndex, mFastDomainSelectionBwThreshold);
    }
    return mStatus;
}

// We know at least one location set is good, because we run locationLevelHealthCheck first.
void OrigStreamingLocationLevel::updateLocationLevel(bool forceSelection, bool enableLogging)
{
    uint32_t locationSetIndex = 0;

    uint32_t maxWeightedThroughput = 0;
    int32_t selectedLocationSetIndex = -1;

    bool selLocation = false;

    for (locationSetIndex = 0; locationSetIndex < mStreamingLocationSetList.size(); ++locationSetIndex)
    {
        SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[locationSetIndex];
        STATUS status = pStreamingLocationSet->getStatus();
        ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();

        INetworkMonitor::PredictionSource predictionSource;
        double confidence;
        uint32_t locationThroughput =
            mNetworkMonitorPtr->getPredictedThroughput( pLocationSetMonitor, mThroughputMeasureWindow, predictionSource, confidence);
        uint32_t weightedLocationThroughput = locationThroughput * pStreamingLocationSet->getWeight();
        DEBUGLOG(LocationSelectorLog,
                "location set: %s, throughput: %u kbps, source: %u, confidence: %f, status: %u",
                pStreamingLocationSet->getLocationSetKey().c_str(),
                locationThroughput, predictionSource, confidence, status);

        if ( (status == TEMP_FAILED_STATE) || (status == PERM_FAILED_STATE) )
        {
            // We need to skip the failed location set.
            continue;
        }
        else if ( selectedLocationSetIndex < 0 )
        {
            // the first non-failed location
            selectedLocationSetIndex = locationSetIndex;
        }

        if ( confidence >= mThroughputHighConfidenceLevel )
        {
            // we have fresh measurement for this location
            if ( locationThroughput > mFastDomainSelectionBwThreshold )
            {
                // this location is fast enough, select it.
                mSelReason = SEL_FAST_SELECTION;
                mCurrentLocationSetIndex = locationSetIndex;
                mStatus = SELECTED_STATE;
                mRecentServerSelReason = IStreamingReporter::SERVER_PERFORMANCE;

                // notify that a location is selected
                if ( enableLogging )
                {
                    DEBUGLOG(LocationSelectorLog,
                            "location selected: indx: %u, fastselection, ",
                            mCurrentLocationSetIndex);
                    notifyLocationSelected(mCurrentLocationSetIndex, mFastDomainSelectionBwThreshold);
                }
                selLocation = true;

                break;
            }
            else if (weightedLocationThroughput > maxWeightedThroughput)
            {
                DEBUGLOG(LocationSelectorLog,
                        "updated max weighted bw: (%u,%u) kbps, indx: %u",
                        weightedLocationThroughput, maxWeightedThroughput,
                        locationSetIndex);
                maxWeightedThroughput = weightedLocationThroughput;
                selectedLocationSetIndex = locationSetIndex;
            }

        }
        else
        {
            // We don't have fresh measurement for this location set yet.
            // So we need to finish measuring this location first
            mCurrentLocationSetIndex = locationSetIndex;
            mRecentServerSelReason = IStreamingReporter::SERVER_PROBE;
            mStatus = MEASURING_STATE;
            break;
        }

        if (locationSetIndex == mStreamingLocationSetList.size() - 1)
        {
            if ( selectedLocationSetIndex < 0 )
                selectedLocationSetIndex = 0;
            // We have measured every location set
            mStatus = SELECTED_STATE;
            mCurrentLocationSetIndex = selectedLocationSetIndex;

            mRecentServerSelReason = IStreamingReporter::SERVER_PERFORMANCE;
            mSelReason = SEL_MAXWEIGHTED_BW;
            // notify that a location is selected
            if (enableLogging)
            {
                DEBUGLOG(LocationSelectorLog,
                        "location selected: indx: %u, maxweightedbw",
                        mCurrentLocationSetIndex);
                notifyLocationSelected(mCurrentLocationSetIndex, mFastDomainSelectionBwThreshold);
            }
            selLocation = true;
        }

    }

    if ( selectedLocationSetIndex < 0 )
        selectedLocationSetIndex = 0;
    if ( !selLocation && forceSelection )
    {
        mStatus = SELECTED_STATE;
        mCurrentLocationSetIndex = selectedLocationSetIndex;
        DEBUGLOG(LocationSelectorLog,
                "location selected: indx: %u, force",
                mCurrentLocationSetIndex);
        if ( mFastplay && enableLogging )
        {
            mSelReason = SEL_MCQUEEN;
            notifyLocationSelected(mCurrentLocationSetIndex, mFastDomainSelectionBwThreshold);
        }
    }

}

void OrigStreamingLocationLevel::checkSelection()
{
    if ( mStatus == MEASURING_STATE )
        updateLocationLevel(true, false);
    else
        mCurrentLocationSetIndex = locationLevelHealthCheck();
}

uint32_t OrigStreamingLocationLevel::foundMatchedLocation(std::string const& locationSetKey)
{
    uint32_t locationSetIndex = 0;
    for (locationSetIndex = 0; locationSetIndex < mStreamingLocationSetList.size(); ++locationSetIndex)
    {
        if ( mStreamingLocationSetList[locationSetIndex]->getLocationSetKey() == locationSetKey )
            break;
    }
    return locationSetIndex;
}
