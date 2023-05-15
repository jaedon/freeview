/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "StreamingLocationLevel.h"

#include <vector>
#include <nrdbase/Variant.h>
#include "SingleStreamingLocationSet.h"
#include "SimpleLocationSelector.h"
#include "INetworkMonitor.h"
#include "ServingStream.h"
#include "ServingUrl.h"

/**
 * Refractoring:
 * 1. Split StreamingLocationSet into StreamingLocationSet which only tracks location set's network performance and
 *          UrlSet which contains multiple urls in one location set and tracks the http error
 */

using namespace netflix;
using namespace netflix::ase;
using namespace std;

StreamingLocationLevel::StreamingLocationLevel(shared_ptr<SimpleLocationSelector> pLocationSelector,
                           shared_ptr<ServingStream> pServingStream,
                           IAseConfigParameterPtr pConfigParameter,
                           uint32_t level, bool lowGradeFlag) :
                                        mLocationSelectorPtr(pLocationSelector),
                                        mServingStreamPtr(pServingStream),
                                        mConfigParameterPtr(pConfigParameter),
                                        mLevel(level), mLowGradeFlag(lowGradeFlag),
                                        mFastplay(false),
                                        mConsumerStateChange(false),
                                        mFreezeSwitch(false),
                                        mPrevStatus(INIT_STATE), mStatus(INIT_STATE),
                                        mCurrentLocationSetIndex(0),
                                        mPrevLocationSetIndex(0),
                                        mPrevCdnId(0),
                                        mRecentServerSelReason(IStreamingReporter::SERVER_UNKNOWN),
                                        mProbingIndex(0),
                                        mSelectedIndex(0),
                                        mTestReason(TEST_STARTUP),
                                        mPreviousMediaConsumerState( IMediaConsumer::MEDIA_CONSUMER_STOPPED_STATE ),
                                        mCurrentMediaConsumerState( IMediaConsumer::MEDIA_CONSUMER_STOPPED_STATE )

{
    mThroughputLowConfidenceLevel =
        mConfigParameterPtr->getParameterAs<double>("throughputLowConfidenceLevel", 0.4);
    mThroughputHighConfidenceLevel =
        mConfigParameterPtr->getParameterAs<double>("throughputHighConfidenceLevel", 0.75);

    mThroughputMeasureWindow =
        mConfigParameterPtr->getParameterAs<uint32_t>("throughputMeasureWindow", DEFAULT_THROUGHPUT_MEASURE_WINDOW);


    Variant variant = mConfigParameterPtr->queryParameter("cdnconfig");
    if ( !variant.isNull() )
    {
        if (variant.isString())
        {
            variant = Variant::fromJSON(variant.string());
        }
        for (Variant::ArrayIterator iter = variant.arrayBegin(); iter != variant.arrayEnd(); ++iter)
        {
            if ( (*iter).isStringMap() )
            {
                shared_ptr<CdnParam> pCdnParam = shared_ptr<CdnParam>(new CdnParam);
                string cdnType;

                for (Variant::StringMapIterator mapIter = (*iter).stringMapBegin();
                     mapIter != (*iter).stringMapEnd();
                     ++mapIter)
                {
                    string key = mapIter->first;
                    if ( key == "cdntype" )
                    {
                        cdnType = mapIter->second.value<string>(0, string());
                    }
                    else if ( key == "connectionFailureThreshold" )
                    {
                        pCdnParam->mConnectionFailureThreshold = mapIter->second.value<uint32_t>(0, 3);
                    }
                    else if ( key == "httpConnectTimeout" )
                    {
                        pCdnParam->mHttpConnectTimeout = mapIter->second.value<uint32_t>(0, 8000);
                    }
                    else if ( key == "httpStallTimeout" )
                    {
                        pCdnParam->mHttpStallTimeout = mapIter->second.value<uint32_t>(0, 8000);
                    }
                }

                if ( cdnType != "" )
                {
                    mCdnParamMap[cdnType] = pCdnParam;
                    DEBUGLOG(LocationSelectorLog, "cdnparam: type: %s, value: %u",
                                        cdnType.c_str(), pCdnParam->mConnectionFailureThreshold);
                }
            }
        }

    }

}


// reset the state out of the temp failure mode
void StreamingLocationLevel::resetTempFailures()
{
    if ( mStatus == TEMP_FAILED_STATE )
    {
        mCurrentLocationSetIndex = locationLevelHealthCheck();
        for ( vector<SingleStreamingLocationSetPtr>::iterator iter = mStreamingLocationSetList.begin();
                iter != mStreamingLocationSetList.end();
                ++iter )
        {
            (*iter)->resetTempFailures();
        }
        mStatus = SELECTED_STATE;
        mSelectedIndex = mCurrentLocationSetIndex;
        mFreezeSwitch = true;
    }

}

void StreamingLocationLevel::reportTempNetworkFailure(AseUrl::HostPort const& destination)
{
    for ( vector<SingleStreamingLocationSetPtr>::iterator iter = mStreamingLocationSetList.begin();
            iter != mStreamingLocationSetList.end();
            ++iter )
    {
        (*iter)->reportTempNetworkFailure(destination);
    }
}


void StreamingLocationLevel::setFastplay(bool fastplay)
{
    mFastplay = fastplay;
}

void StreamingLocationLevel::enableLowGradeLevel()
{
    if (mLowGradeFlag)
    {
        DEBUGLOG(LocationSelectorLog, "enableLowGradeLevel: status: %u",
                mPrevStatus);
        mStatus = mPrevStatus;
    }
}


void StreamingLocationLevel::disableLowGradeLevel()
{
    // ignore the command if it's not a low grade level
    if ( mLowGradeFlag )
    {
        DEBUGLOG(LocationSelectorLog, "disableLowGradeLevel, status: %u, %u",
                mPrevStatus, mStatus);
        mPrevStatus = mStatus;
        mStatus = NOT_USABLE_STATE;
    }
}

/**
 * potential state change from SELECTED_STATE to TEMP_FAILED_STATE or PERM_FAILED_STATE
 * driven by the underlying network errors received by streaming location sets.
 * 1. If there is at least one location set available, find the next available location set;
 * 2. If there is no locatino set available, find the first temporary failed location set
 * FIXME: handle both the original and the continuous monitoring case
 */
uint32_t StreamingLocationLevel::locationLevelHealthCheck()
{
    SingleStreamingLocationSetPtr pStreamingLocationSet = mStreamingLocationSetList[mCurrentLocationSetIndex];
    STATUS status = pStreamingLocationSet->getStatus();
    if ( (status != TEMP_FAILED_STATE) && (status != PERM_FAILED_STATE))
    {
        return mCurrentLocationSetIndex;
    }

    uint32_t numOfLocations = mStreamingLocationSetList.size();
    uint32_t probingIndex =
                   (mCurrentLocationSetIndex + 1) % numOfLocations;
    int32_t indexForNextValidLocationSet = -1;
    while ( probingIndex != mCurrentLocationSetIndex )
    {
        status = mStreamingLocationSetList[probingIndex]->getStatus();
        DEBUGLOG(LocationSelectorLog,
                 "probing index: %u, current location set indx: %u, status: %u:%u",
                 probingIndex, mCurrentLocationSetIndex,
                 status, mStreamingLocationSetList[mCurrentLocationSetIndex]->getStatus());
        if ( (status != TEMP_FAILED_STATE) && (status != PERM_FAILED_STATE) )
        {
            indexForNextValidLocationSet = probingIndex;
            break;
        }
        else if ( (status == TEMP_FAILED_STATE) && (indexForNextValidLocationSet < 0) )
        {
            indexForNextValidLocationSet = probingIndex;
        }
        probingIndex = (probingIndex + 1) % numOfLocations;
    }

    if ( indexForNextValidLocationSet < 0 )
        indexForNextValidLocationSet = (mCurrentLocationSetIndex + 1) % numOfLocations;

    mCurrentLocationSetIndex = indexForNextValidLocationSet;

    if ( mStatus == MEASURING_STATE )
    {
        mProbingIndex = indexForNextValidLocationSet;
    }
    else if ( mStatus == SELECTED_STATE )
    {
        mSelectedIndex = indexForNextValidLocationSet;
    }

    status = mStreamingLocationSetList[indexForNextValidLocationSet]->getStatus();
    DEBUGLOG(LocationSelectorLog,
            "indexForNextValidLocationSet: %u, status: %u",
            indexForNextValidLocationSet, status);
    if ( (status == TEMP_FAILED_STATE) || (status == PERM_FAILED_STATE) )
    {
        if ( mStatus != NOT_USABLE_STATE )
            mStatus = status;
        else
            mPrevStatus = status;
    }

    return indexForNextValidLocationSet;
}

void StreamingLocationLevel::initializeLocationLevel(ManifestUrls const& manifestUrls,
                                                     std::set<ManifestServingLocationPtr, DereferenceLess> const& servingLocations,
                                                     INetworkMonitorPtr pNetworkMonitor)
{
    // construct mStreamingLocationSetList given the pNetworkMonitor
    mStreamingLocationSetList.clear();

    mNetworkMonitorPtr = pNetworkMonitor;

    SingleStreamingLocationSetPtr pStreamingLocationSet;
    for ( set<ManifestServingLocationPtr, DereferenceLess>::const_iterator locationIter = servingLocations.begin();
            locationIter != servingLocations.end();
            ++locationIter )
    {
        ManifestUrls urlsForOneLocationSet;
        for ( vector<ConstManifestUrlPtr>::const_iterator urlIter = manifestUrls.begin();
                urlIter != manifestUrls.end();
                ++urlIter )
        {
            if ( (*urlIter)->getLocationKey() == (*locationIter)->getLocationKey() )
            {
                urlsForOneLocationSet.push_back(*urlIter);
            }
        }
        if ( !urlsForOneLocationSet.empty() )
        {
            SingleStreamingLocationSet::construct(
                        mServingStreamPtr.lock(), mCdnParamMap,
                        urlsForOneLocationSet, *locationIter, pNetworkMonitor,
                        mLowGradeFlag, mConfigParameterPtr, pStreamingLocationSet);
            mStreamingLocationSetList.push_back(pStreamingLocationSet);
        }
    }


    if ( mLowGradeFlag )
    {
        mPrevStatus = SELECTED_STATE;
        mStatus = NOT_USABLE_STATE;
    }
    else
    {
        mPrevStatus = INIT_STATE;
        mStatus = INIT_STATE;
    }

}


AseErrorCode
StreamingLocationLevel::getPreferredLocationSet(shared_ptr<IStreamingLocationSet>& pStreamingLocationSet,
                                                ILocationSelector::LocationSelectionRequestSource /**requestSource*/)
{
    // dummy implmentation
    pStreamingLocationSet = mStreamingLocationSetList[0];
    return AS_NO_ERROR;
}

IStreamingLocationLevel::STATUS StreamingLocationLevel::getStatus()
{

    return mStatus;
}

void StreamingLocationLevel::checkSelection()
{
    return;
}

void StreamingLocationLevel::consumerStateChange()
{
    mConsumerStateChange = true;
}

void StreamingLocationLevel::notifyLocationSelected(uint32_t locationSetIndex, uint32_t fastselthreshold)
{
    SingleStreamingLocationSetPtr pSelLocationSet = mStreamingLocationSetList[locationSetIndex];
    ILocationSetMonitorPtr pSelLocationSetMonitor = pSelLocationSet->getLocationSetMonitor();

    INetworkMonitor::PredictionSource source;
    double confidence;
    uint32_t locationThroughput =
        mNetworkMonitorPtr->getPredictedThroughput(pSelLocationSetMonitor, mThroughputMeasureWindow, source,confidence);
    uint32_t avgConnectTime, varConnectTime, numberOfConnectionFailures;
    pSelLocationSetMonitor->connectionStat(avgConnectTime, varConnectTime, numberOfConnectionFailures);

    std::string testreason;
    switch (mTestReason)
    {
        case TEST_PERFPROBE:
            testreason = "perfprobe";
            break;
        case TEST_STARTUP:
            testreason = "startup";
            break;
        case TEST_REBUFFER:
            testreason = "rebuffer";
            break;
        case TEST_SEEK:
            testreason = "seek";
            break;
        case TEST_DLFAIL:
            testreason = "dlfail";
            break;
        default:
            break;
    }

    std::string selreason;
    switch (mSelReason)
    {
        case SEL_MAXWEIGHTED_BW:
            selreason = "maxweightedbw";
            break;
        case SEL_FAST_SELECTION:
            selreason = "fastselection";
            break;
        case SEL_REUSE_PREVIOUS:
            selreason = "reuseprevious";
            break;
        case SEL_NEXT_DOMAIN:
            selreason = "nextdomain";
            break;
        case SEL_ONLY_CHOICE:
            selreason = "onlychoice";
            break;
        case SEL_MCQUEEN:
            selreason = "mcqueen";
            break;
        case SEL_TO_LOW_GRADE:
            selreason = "tolowgrade";
            break;
        case SEL_FROM_LOW_GRADE:
            selreason = "fromlowgrade";
            break;
        default:
            break;
    }

    ServingStreamPtr pServingStream = mServingStreamPtr.lock();
    if ( pServingStream )
    {
        pServingStream->notifyLocationSelected( pSelLocationSet->getLocationSetKey(),
                                          pSelLocationSet->getDownloadableUrl(),
                                          avgConnectTime, locationThroughput,
                                          testreason, selreason,
                                          fastselthreshold );
    }

    DEBUGLOG( LocationSelectorLog,
              "selected a location, key: %s, stream key: %s, "
              "Test Reason: %s, Select Reason: %s",
              pSelLocationSet->getLocationSetKey().c_str(),
              pServingStream->streamKey().c_str(),
              testreason.c_str(),
              selreason.c_str() );
}

void StreamingLocationLevel::checkNewServerSelected(IStreamingLocationSetPtr pSelStreamingLocationSet)
{
    shared_ptr<SingleStreamingLocationSet> pPrevStreamingLocationSet = mStreamingLocationSetList[mPrevLocationSetIndex];

    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();

    /** notify selected server changed */
    if ( pLocationSelector != SimpleLocationSelectorPtr() )
    {
        if ( pSelStreamingLocationSet->getLocationSetKey() == pPrevStreamingLocationSet->getLocationSetKey() )
        {
            if ( pSelStreamingLocationSet->getPrimaryCdnId() != mPrevCdnId )
            {
                pLocationSelector->serverSelected(IStreamingReporter::SERVER_SERVERFAILOVER);
            }
        }
        else
        {
            pLocationSelector->serverSelected(mRecentServerSelReason);
        }
    }


    mPrevLocationSetIndex = mCurrentLocationSetIndex;
    mPrevCdnId = pSelStreamingLocationSet->getPrimaryCdnId();
    return;
}
