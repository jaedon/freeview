/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
*/

#include "ContStreamingLocationLevel.h"

#include "SimpleLocationSelector.h"
#include "INetworkMonitor.h"
#include "SingleStreamingLocationSet.h"
#include "ServingStream.h"

#include <nrdase/IMediaConsumer.h>

#include <limits>

/**
 * state transition between MEASURING_STATE and SELECTED_STATE
 * MEASURING_STATE --> SELECTED_STATE
 *          condition : buffer level < throughputProbingExitThreshold OR no location to probe
 * MEASURING_STATE --> MEASURING_STATE
 *          condition : buffer level >= throughputProbingExitThreshold AND has at least one location to probe
 * SELECTED_STATE --> MEASURING_STATE
 *          condition : buffer level >= throughputProbingEnterThreshold AND has at least one location to probe
 * SELECTED_STATE --> SELECTED_STATE
 *          condition : buffer level < throughputProbingEnterThreshold OR no location to probe
 *
 *  The class maintains three indexes:
 *  1. mCurrentLocationSetIndex points to the current location set
 *  2. mSelectedIndex points to the selected location set
 *  3. mProbingIndex points to the location to be probed next
 */

using namespace netflix::ase;
using namespace std;

ContStreamingLocationLevel::ContStreamingLocationLevel(
                                shared_ptr<SimpleLocationSelector> pLocationSelector,
                                shared_ptr<ServingStream> pServingStream,
                                ManifestUrls const& manifestUrls,
                                std::set<ManifestServingLocationPtr, DereferenceLess> servingLocations,
                                INetworkMonitorPtr pNetworkMonitor,
                                IAseConfigParameterPtr pConfigParameter,
                                uint32_t level, bool lowGradeFlag) :
                                        StreamingLocationLevel(pLocationSelector, pServingStream,
                                                               pConfigParameter,
                                                               level, lowGradeFlag),
                                        mProbingStartTime(AseTimeVal::INFINITE),
                                        mPrevCheckTime(AseTimeVal::INFINITE)

{
    initializeLocationLevel(manifestUrls, servingLocations, pNetworkMonitor);

    mThroughputProbingEnterThreshold =
        mConfigParameterPtr->getParameterAs<uint32_t>("throughputProbingEnterThreshold", DEFAULT_THROUGHPUT_ENTER_THRESHOLD);
    mThroughputProbingExitThreshold =
        mConfigParameterPtr->getParameterAs<uint32_t>("throughputProbingExitThreshold", DEFAULT_THROUGHPUT_EXIT_THRESHOLD);

    mLocationProbingTimeout =
        mConfigParameterPtr->getParameterAs<uint32_t>("locationProbingTimeout", 10000);
    mLocationSelectionCheckTimeout =
        mConfigParameterPtr->getParameterAs<uint32_t>("locationSelectionCheckTimeout", 0);

    int const intmax = std::numeric_limits<int32_t>::max();

    mFinalLocationSelectionBwThreshold =
        mConfigParameterPtr->getParameterAs<uint32_t>("finalLocationSelectionBwThreshold", 1000, intmax, 10000);

    if ( mThroughputProbingExitThreshold >= mThroughputProbingEnterThreshold )
    {
        mThroughputProbingEnterThreshold = 40000;
        mThroughputProbingExitThreshold = 20000;
        DEBUGLOG(LocationSelectorLog,
                "WARNING: correct parameters: throughputProbingEnterThreshold: %u, throughputProbingExitThreshold: %u",
                mThroughputProbingEnterThreshold, mThroughputProbingExitThreshold);
    }
}



AseErrorCode
ContStreamingLocationLevel::getPreferredLocationSet(IStreamingLocationSetPtr& pStreamingLocationSet,
                                                    ILocationSelector::LocationSelectionRequestSource requestSource)
{
    DEBUGLOG(LocationSelectorLog,
            "getPreferredLocationSet request source: %u, status: %u",
            requestSource, mStatus);
    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if ( pLocationSelector == SimpleLocationSelectorPtr() )
        return AS_NO_ERROR;

    bool locationSelected = pLocationSelector->locationSelectionComplete();
    uint32_t seqNo = pLocationSelector->locationSelectionSeqNo();

    IMediaConsumer::State previousState, currentState;
    pLocationSelector->mediaConsumerState(previousState, currentState);

    if ( locationSelected )
    {
        if ( mStatus == INIT_STATE )
        {
            initialLocationSelection();
        }
    }
    else
    {
        if ( mStatus == INIT_STATE )
        {
            initialLocationSelection();
            if ( (seqNo == 0) && !locationSelected )
            {
                mTestReason = TEST_STARTUP;
                mSelReason = SEL_MAXWEIGHTED_BW;
                // notify location selected for initial buffering
                notifyLocationSelected(mSelectedIndex, 0);
            }
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
                            // notify location selected for seek
                            notifyLocationSelected(mSelectedIndex, 0);
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

                mPreviousMediaConsumerState = previousState;
                mCurrentMediaConsumerState = currentState;
                mConsumerStateChange = false;
            }
        }
    }

    if ( ( mStatus != TEMP_FAILED_STATE ) && (mStatus != PERM_FAILED_STATE) && !mFreezeSwitch )
    {
        if ( requestSource == ILocationSelector::VIDEO_DOWNLOADING )
        {
            // Only change the state to MEASURING_STATE,
            // if the player is going to download some data from this ServingStream
            updateLocationMonitoring();
        }
    }

    AseTimeVal now = AseTimeVal::now();
    if ( (mLocationSelectionCheckTimeout > 0) && (mPrevCheckTime + AseTimeVal::fromMS(mLocationSelectionCheckTimeout) < now) )
    {
        uint32_t mPrevSelIndex = mSelectedIndex;
        selectLocationByPerformance();
        mPrevCheckTime = now;
        if ( mStatus == SELECTED_STATE )
        {
            mCurrentLocationSetIndex = mSelectedIndex;
        }
        if ( mSelectedIndex != mPrevSelIndex )
        {
            mTestReason = TEST_PERFPROBE;
            mSelReason = SEL_MAXWEIGHTED_BW;
            mRecentServerSelReason = IStreamingReporter::SERVER_PERFORMANCE;
            notifyLocationSelected(mSelectedIndex, 0);
        }
    }

    pStreamingLocationSet = mStreamingLocationSetList[mCurrentLocationSetIndex];
    DEBUGLOG(LocationSelectorLog, "getPreferredLocationSet: %s", pStreamingLocationSet->getLocationSetKey().c_str());
    checkNewServerSelected(pStreamingLocationSet);

    return AS_NO_ERROR;
}

IStreamingLocationLevel::STATUS ContStreamingLocationLevel::getStatus()
{
    uint32_t currentSelIndex = mSelectedIndex;
    uint32_t currentProbingIndex = mProbingIndex;
    locationLevelHealthCheck();
    if ( mStatus == SELECTED_STATE )
    {
        if ( currentSelIndex != mSelectedIndex )
        {
            mSelReason = SEL_NEXT_DOMAIN;
            mTestReason = TEST_DLFAIL;
            mRecentServerSelReason = IStreamingReporter::SERVER_LOCATIONFAILOVER;
            notifyLocationSelected(mSelectedIndex, 0);
            mCurrentLocationSetIndex = mSelectedIndex;
        }
    }
    else if ( mStatus == MEASURING_STATE )
    {
        if ( currentProbingIndex != mProbingIndex )
        {
            mCurrentLocationSetIndex = mProbingIndex;
        }
    }

    return mStatus;
}

void ContStreamingLocationLevel::updateLocationMonitoring()
{
    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if ( pLocationSelector == SimpleLocationSelectorPtr() )
        return;

    DEBUGLOG(LocationSelectorLog, "updateLocationMonitoring");

    IMediaConsumer::State consumerState = pLocationSelector->getMediaConsumerState();
    uint32_t playableLength = pLocationSelector->getMediaPlayableLength();
    bool foundIndex;

    shared_ptr<SingleStreamingLocationSet> pCurrentLocationSet;

    uint32_t mPrevSelIndex = mSelectedIndex;

    AseTimeVal now = AseTimeVal::now();

    if ( mStatus == MEASURING_STATE )
    {
        if ( ((playableLength <= mThroughputProbingExitThreshold) ||
                    (now > mProbingStartTime + AseTimeVal::fromMS(mLocationProbingTimeout)) ) &&
                (mCurrentMediaConsumerState != IMediaConsumer::MEDIA_CONSUMER_REBUFFERING_STATE) )
        {
            // If the playable length is too low, or it has taken a long time to probe this location,
            // we exit the probing state immediately
            updateCacheSelection(mCurrentLocationSetIndex);
            SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[mCurrentLocationSetIndex];
            ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();
            pLocationSetMonitor->probeDone();
            mStatus = SELECTED_STATE;
            pLocationSelector->updateLocationProbing(false);
            mCurrentLocationSetIndex = mSelectedIndex;
            if ( mSelectedIndex == mPrevSelIndex )
                mRecentServerSelReason = IStreamingReporter::SERVER_PROBEABORTED;


            DEBUGLOG(LocationSelectorLog,
                    "Exit probing: playableLength: %u, updated location set index: %u",
                    playableLength, mCurrentLocationSetIndex);
        }

        if ( completeProbing(mCurrentLocationSetIndex) )
        {
            updateCacheSelection(mCurrentLocationSetIndex);
            mProbingIndex = getNextProbingIndex(foundIndex);
            if ( foundIndex )
            {
                // have one more location to probe. Stay in the probing state
                mCurrentLocationSetIndex = mProbingIndex;
                pCurrentLocationSet = mStreamingLocationSetList[mCurrentLocationSetIndex];
                mRecentServerSelReason = IStreamingReporter::SERVER_PROBE;
                mProbingStartTime = AseTimeVal::now();
                DEBUGLOG(LocationSelectorLog,
                        "Continue probing: current index: %u, probing index: %u, selected index: %u",
                        mCurrentLocationSetIndex, mProbingIndex, mSelectedIndex);

            }
            else
            {
                mStatus = SELECTED_STATE;
                mCurrentLocationSetIndex = mSelectedIndex;
                if ( mSelectedIndex == mPrevSelIndex )
                    mRecentServerSelReason = IStreamingReporter::SERVER_PROBEDONE;
                DEBUGLOG(LocationSelectorLog,
                        "Exit probing: have probed all locations: "
                        "current index: %u, probing index: %u, selected index: %u",
                        mCurrentLocationSetIndex, mProbingIndex, mSelectedIndex);
                bool locationSelected = pLocationSelector->locationSelectionComplete();
                pLocationSelector->updateLocationProbing(false);
                if ( (mCurrentMediaConsumerState == IMediaConsumer::MEDIA_CONSUMER_REBUFFERING_STATE) &&
                        !locationSelected )
                {
                    // notify location selected for rebuffering after all locations are measured
                    mSelReason = SEL_MAXWEIGHTED_BW;
                    notifyLocationSelected(mSelectedIndex, 0);
                }
            }
        }
    }
    else if ( mStatus == SELECTED_STATE )
    {
        // the player only probes other location's throughput when it's playing with a safe buffer
        if ( (consumerState == IMediaConsumer::MEDIA_CONSUMER_PLAYING_STATE)
                && (playableLength > mThroughputProbingEnterThreshold) )
        {
            mProbingIndex = getNextProbingIndex(foundIndex);
            if ( foundIndex )
            {
                // enter probing state
                mCurrentLocationSetIndex = mProbingIndex;
                mStatus = MEASURING_STATE;
                mProbingStartTime = AseTimeVal::now();
                pLocationSelector->updateLocationProbing(true);
                pCurrentLocationSet = mStreamingLocationSetList[mCurrentLocationSetIndex];
                mRecentServerSelReason = IStreamingReporter::SERVER_PROBE;
                DEBUGLOG(LocationSelectorLog,
                         "Enter probing: playableLength: %u, "
                         "current index: %u, probing index: %u, selected index: %u",
                         playableLength, mCurrentLocationSetIndex, mProbingIndex, mSelectedIndex);
            }
        }
    }
    return;
}

bool ContStreamingLocationLevel::eligibleForProbing(uint32_t index)
{
    SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[index];
    ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();
    STATUS status = pStreamingLocationSet->getStatus();

    bool retVal = false;
    if ( (status != TEMP_FAILED_STATE) && (status != PERM_FAILED_STATE) )
    {
        double throughputConfidence = pLocationSetMonitor->getConfidence();
        if ( (throughputConfidence < mThroughputLowConfidenceLevel) &&
                pLocationSetMonitor->eligibleForProbing() )
            retVal = true;
    }

    return retVal;
}

uint32_t ContStreamingLocationLevel::getNextProbingIndex(bool& foundIndex)
{
    // check the current probing index
    foundIndex = false;

    double confidence = 0.0;
    INetworkMonitor::PredictionSource predictionSource;
    uint32_t throughput = calculateThroughput(mSelectedIndex, confidence, predictionSource);
    if ( throughput > mFinalLocationSelectionBwThreshold )
    {
        // The selected location's throughput is already very good
        return 0;
    }


    if ( (mProbingIndex != mSelectedIndex) && eligibleForProbing(mProbingIndex) )
    {
        foundIndex = true;
        return mProbingIndex;
    }

    uint32_t numOfLocationSets = mStreamingLocationSetList.size();
    uint32_t currProbingIndex = (mProbingIndex+1) % numOfLocationSets;

    uint32_t probingIndex = currProbingIndex;
    // We are looking for a new location to probe, which is
    // not the selected location, does not fail, and does not have fresh bandwidth measurement
    while ( currProbingIndex != mProbingIndex )
    {
        if ( (currProbingIndex != mSelectedIndex) && eligibleForProbing(currProbingIndex) )
        {
            probingIndex = currProbingIndex;
            foundIndex = true;
            break;
        }
        currProbingIndex = (currProbingIndex+1) % numOfLocationSets;
    }

    return probingIndex;
}

bool ContStreamingLocationLevel::completeProbing(uint32_t index)
{
    SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[index];
    ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();
    STATUS status = pStreamingLocationSet->getStatus();

    bool retVal = false;
    double throughputConfidence = 0.0;
    if ( (status == TEMP_FAILED_STATE) || (status == PERM_FAILED_STATE) )
    {
        throughputConfidence = 1.0;
        pLocationSetMonitor->probeDone();
        retVal = true;
    }
    else
    {
        throughputConfidence = pLocationSetMonitor->getConfidence();
        if ( throughputConfidence > mThroughputHighConfidenceLevel )
        {
            pLocationSetMonitor->probeDone();
            retVal = true;
        }
    }
    DEBUGLOG(LocationSelectorLog,
            "completeProbing: %u, throughputConfidence: %f, status: %u",
            retVal, throughputConfidence, status);
    return retVal;
}

void ContStreamingLocationLevel::updateCacheSelection(uint32_t probingIndex)
{
    double throughputConfidence;
    INetworkMonitor::PredictionSource predictionSource;
    uint32_t weightedLocationThroughput = calculateWeightedThroughput(probingIndex, throughputConfidence, predictionSource);
    if ( (throughputConfidence < mThroughputLowConfidenceLevel) || (weightedLocationThroughput == 0) )
    {
        return;
    }

    double selThroughputConfidence;
    uint32_t selWeightedThroughput = calculateWeightedThroughput(mSelectedIndex, selThroughputConfidence, predictionSource);
    if ( weightedLocationThroughput > selWeightedThroughput )
    {
        // location selection event
        mSelectedIndex = probingIndex;
        mTestReason = TEST_PERFPROBE;
        mSelReason = SEL_MAXWEIGHTED_BW;
        mRecentServerSelReason = IStreamingReporter::SERVER_PERFORMANCE;
        notifyLocationSelected(mSelectedIndex, 0);
    }
    DEBUGLOG(LocationSelectorLog,
            "updateCacheSelection: current: %u:%f, new: %u:%f",
            selWeightedThroughput, selThroughputConfidence,
            weightedLocationThroughput, throughputConfidence);
    return;
}


void ContStreamingLocationLevel::checkSelection()
{
    uint32_t locationSetIndex = 0;

    double confidence;
    INetworkMonitor::PredictionSource predictionSource;
    uint32_t maxWeightedThroughput = calculateWeightedThroughput(mSelectedIndex, confidence, predictionSource);

    for (locationSetIndex = 0; locationSetIndex < mStreamingLocationSetList.size(); ++locationSetIndex)
    {
        uint32_t weightedLocationThroughput = calculateWeightedThroughput(locationSetIndex, confidence, predictionSource);

        if ( (confidence >= mThroughputHighConfidenceLevel) && (weightedLocationThroughput > maxWeightedThroughput) )
        {
            maxWeightedThroughput = weightedLocationThroughput;
            mSelectedIndex = locationSetIndex;
            if ( mStatus == SELECTED_STATE )
                mCurrentLocationSetIndex = mSelectedIndex;
        }
    }

}

uint32_t
ContStreamingLocationLevel::calculateWeightedThroughput(uint32_t locationSetIndex, double& confidence,
                                                        INetworkMonitor::PredictionSource& predictionSource)
{
    SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[locationSetIndex];
    STATUS status = pStreamingLocationSet->getStatus();
    uint32_t weightedLocationThroughput = 0;
    if ( (status != TEMP_FAILED_STATE) && (status != PERM_FAILED_STATE) )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();

        uint32_t locationThroughput =
            mNetworkMonitorPtr->getPredictedThroughput(pLocationSetMonitor, mThroughputMeasureWindow, predictionSource,confidence);
        weightedLocationThroughput = locationThroughput * pStreamingLocationSet->getWeight();
    }
    return weightedLocationThroughput;
}

uint32_t
ContStreamingLocationLevel::calculateThroughput(uint32_t locationSetIndex, double& confidence,
                                                INetworkMonitor::PredictionSource& predictionSource)
{
    SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[locationSetIndex];
    STATUS status = pStreamingLocationSet->getStatus();
    uint32_t throughput = 0;
    if ( (status != TEMP_FAILED_STATE) && (status != PERM_FAILED_STATE) )
    {
        ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();
        throughput =
            mNetworkMonitorPtr->getPredictedThroughput(pLocationSetMonitor, mThroughputMeasureWindow, predictionSource, confidence);
    }
    return throughput;
}

void ContStreamingLocationLevel::selectLocationByPerformance()
{
    DEBUGLOG(LocationSelectorLog, "selectLocationByPerformance");
    uint32_t maxWeightedThroughput = 0;
    double confidence = 0.0;
    uint32_t weightedLocationThroughput = 0;
    for (uint32_t locationSetIndex = 0; locationSetIndex < mStreamingLocationSetList.size(); ++locationSetIndex)
    {
        SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[locationSetIndex];
        STATUS status = pStreamingLocationSet->getStatus();
        if ( (status == TEMP_FAILED_STATE) || (status == PERM_FAILED_STATE) )
            continue;

        INetworkMonitor::PredictionSource source;
        weightedLocationThroughput = calculateWeightedThroughput(locationSetIndex, confidence, source);

        if ( (confidence >= mThroughputLowConfidenceLevel) && (weightedLocationThroughput > maxWeightedThroughput) )
        {
            mSelectedIndex = locationSetIndex;
            maxWeightedThroughput = weightedLocationThroughput;
        }
    }

    DEBUGLOG(LocationSelectorLog, "selectLocationByPerformance: selected index: %u, weighted bandwidth: %u, confidence: %f",
                                  mSelectedIndex, weightedLocationThroughput, confidence);
 }

void ContStreamingLocationLevel::initialLocationSelection()
{
    mCurrentLocationSetIndex = 0;
    mSelectedIndex = 0;
    mProbingIndex = 0;

    selectLocationByPerformance();

    mProbingIndex = (mSelectedIndex + 1) % mStreamingLocationSetList.size();
    mCurrentLocationSetIndex = mSelectedIndex;
    mStatus = SELECTED_STATE;

    mPrevCheckTime = AseTimeVal::now();

    DEBUGLOG(LocationSelectorLog,
            "initialLocationSelection: status: %u, selected location index: %u "
            "probing index: %u, key: %s",
            mStatus, mSelectedIndex, mProbingIndex,
            mStreamingLocationSetList[mSelectedIndex]->getLocationSetKey().c_str());

    return;
}
