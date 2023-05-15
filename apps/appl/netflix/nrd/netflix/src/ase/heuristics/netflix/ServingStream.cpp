/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ServingStream.h"

#include "SimpleLocationSelector.h"
#include "IStreamingLocationSet.h"
#include "IStreamingLocationLevel.h"
#include "INetworkMonitor.h"
#include "OrigStreamingLocationLevel.h"
//#include "MultiStreamingLocationLevel.h"
#include "ContStreamingLocationLevel.h"
#include "ServingUrl.h"

using namespace netflix::ase;
using namespace std;
using namespace netflix;

AseErrorCode ServingStream::construct(
                  shared_ptr<SimpleLocationSelector> pLocationSelector,
                  MediaType mediaType,
                  ManifestStreamPtr pManifestStream,
                  std::vector<ManifestServingLocationPtr> const& servingLocations,
                  std::map<uint32_t, ManifestServerPtr> const& servers,
                  shared_ptr<INetworkMonitor> pNetworkMonitor,
                  IAseConfigParameterPtr pConfigParameter,
                  ServingStreamPtr& pServingStream)
{
    pServingStream.reset( new ServingStream( pLocationSelector,
                                             mediaType,
                                             pManifestStream,
                                             servingLocations,
                                             servers,
                                             pNetworkMonitor,
                                             pConfigParameter) );
    pServingStream->initialize();
    return AS_NO_ERROR;
}

ServingStream::ServingStream(shared_ptr<SimpleLocationSelector> pLocationSelector,
                             MediaType mediaType,
                             ManifestStreamPtr pManifestStream,
                             vector<ManifestServingLocationPtr> const& servingLocations,
                             map<uint32_t, ManifestServerPtr> const& servers,
                             INetworkMonitorPtr pNetworkMonitor,
                             IAseConfigParameterPtr pConfigParameter)
                        :   mLocationSelectorPtr(pLocationSelector),
                            mMediaType(mediaType),
                            mManifestStreamPtr(pManifestStream),
                            mServingLocations(servingLocations),
                            mServers(servers),
                            mStreamKey(pManifestStream->getStreamKey()),
                            mIgnoreLevelChange(true),
                            mNetworkMonitorPtr(pNetworkMonitor),
                            mConfigParamterPtr(pConfigParameter),
                            mLocationSelectionSeqNo(0),
                            mLastNetworkErrorCode(AS_NO_ERROR),
                            mLastHttpStatusCode(200)
{
}

void ServingStream::initialize()
{
    mStreamingLocationLevelList.clear();
    ManifestUrls manifestUrls = mManifestStreamPtr->getUrls();

    // make sure that manifest urls are sorted by levels first and then by rank
#if 0
    DEBUGLOG(LocationSelectorLog,
            "list all urls for stream: %u kbps",
            mManifestStreamPtr->getNominalBitrate());
    for ( ManifestUrls::const_iterator urlIter = manifestUrls.begin();
            urlIter != manifestUrls.end();
            ++urlIter )
    {
        DEBUGLOG(LocationSelectorLog,
                "url: %s, cdn id: %u",
                (*urlIter)->getUrl().str().c_str(),
                (*urlIter)->getCdnId());
    }
#endif

    uint32_t currLevel =
        getManifestLocation(manifestUrls[0]->getLocationKey(), mServingLocations)->getLevel();

    mMultiLocationStreaming = mConfigParamterPtr->getParameterAs<bool>(
                                    "multiCdnSelector", false);
    mCacheSelectionAlgorithmFlavor = mConfigParamterPtr->getParameterAs<uint32_t>(
                                    "cacheSelectionAlgorithmFlavor", 0);

    set<ManifestServingLocationPtr, DereferenceLess> locationsForCurrentLevel;
    ManifestUrls urlsForCurrentLevel;

    set<ManifestServingLocationPtr, DereferenceLess> lowGradeLocations;
    ManifestUrls lowGradeUrls;

    uint32_t numOfNormalLevels = 0;

    for ( uint32_t urlIndex = 0; urlIndex < manifestUrls.size(); ++urlIndex )
    {
        ConstManifestUrlPtr pManifestUrl = manifestUrls[urlIndex];
        ManifestServingLocationPtr pManifestLocation =
                        getManifestLocation( pManifestUrl->getLocationKey(), mServingLocations );
        uint32_t level = pManifestLocation->getLevel();

        map<uint32_t, ManifestServerPtr>::const_iterator serverIter = mServers.find(pManifestUrl->getCdnId());
        if ( serverIter != mServers.end() )
        {
            ManifestServerPtr pServer = serverIter->second;
            if (pServer->getLowGradeFlag())
            {
                lowGradeLocations.insert(pManifestLocation);
                lowGradeUrls.push_back(pManifestUrl);
                continue;
            }
        }

        if ( (level != currLevel) && (!urlsForCurrentLevel.empty()) )
        {
            constructLocationLevel( urlsForCurrentLevel, locationsForCurrentLevel,
                                    currLevel, false );
            urlsForCurrentLevel.clear();
            locationsForCurrentLevel.clear();
            currLevel = level;
            ++numOfNormalLevels;
        }

        urlsForCurrentLevel.push_back(pManifestUrl);
        locationsForCurrentLevel.insert(pManifestLocation);
    }

    if ( !urlsForCurrentLevel.empty() )
    {
        // construct the last level
        ++numOfNormalLevels;
        constructLocationLevel( urlsForCurrentLevel, locationsForCurrentLevel,
                                currLevel, false );
    }

    bool lowGradeLevel;
    if ( numOfNormalLevels )
    {
        lowGradeLevel = true;
    }
    else
    {
        ERRORLOG(LocationSelectorLog,
                "All servers are low grade servers for stream: %s",
                mStreamKey.c_str());
        lowGradeLevel = false;
    }

    if ( !lowGradeUrls.empty() )
    {
        // construct the low grade cache
        constructLocationLevel( lowGradeUrls, lowGradeLocations,
                                0, lowGradeLevel );
    }

    if ( mStreamingLocationLevelList.empty() )
    {
        ERRORLOG(LocationSelectorLog,
                "No valid servers for stream: %s",
                mStreamKey.c_str());
    }

    mCurrentStreamingLocationLevel = mStreamingLocationLevelList.front();

}

ServingStream::~ServingStream()
{
}

AseErrorCode
ServingStream::getPreferredLocationSet(IStreamingLocationSetPtr& pStreamingLocationSet,
                                       ILocationSelector::LocationSelectionRequestSource requestSource)
{
    IStreamingLocationLevelPtr pStreamingLocationLevel = IStreamingLocationLevelPtr();
    IStreamingLocationLevelPtr pStreamingLocationLevelWithTempFailure = IStreamingLocationLevelPtr();

    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if ( pLocationSelector == SimpleLocationSelectorPtr() )
        return AS_NO_ERROR;

    if ( mLocationSelectionSeqNo < pLocationSelector->locationSelectionSeqNo() )
    {
        DEBUGLOG(LocationSelectorLog,
                "stream key: %s, bitrate: %u, location selection seq no: %u:%u, check location selection",
                mStreamKey.c_str(), mManifestStreamPtr->getNominalBitrate(),
                mLocationSelectionSeqNo, pLocationSelector->locationSelectionSeqNo());
        for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
                iter != mStreamingLocationLevelList.end();
                ++iter )
        {
            (*iter)->checkSelection();
        }
        mLocationSelectionSeqNo = pLocationSelector->locationSelectionSeqNo();
    }

    for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
            iter != mStreamingLocationLevelList.end();
            ++iter )
    {
        IStreamingLocationLevel::STATUS pLevelStatus = (*iter)->getStatus();
#if 0
        DEBUGLOG(LocationSelectorLog,
                "key: %s, level: %u, check status: %u",
                mStreamKey.c_str(),
                (*iter)->getLevel(), pLevelStatus);
#endif
        switch (pLevelStatus)
        {
            case IStreamingLocationLevel::NOT_USABLE_STATE:
                break;
            case IStreamingLocationLevel::TEMP_FAILED_STATE:
                if ( pStreamingLocationLevelWithTempFailure == IStreamingLocationLevelPtr() )
                {
                    // record the first level with temp_failed_state
                    pStreamingLocationLevelWithTempFailure = *iter;
                }
                break;
            case IStreamingLocationLevel::PERM_FAILED_STATE:
                break;
            default:
                pStreamingLocationLevel = *iter;
                break;
        }
        if ( pStreamingLocationLevel != IStreamingLocationLevelPtr() )
        {
            // got a good level
            break;
        }
    }

    if ( pStreamingLocationLevel == IStreamingLocationLevelPtr() )
    {
        // Every location level is failed
        pLocationSelector->reportLocationFailure(mStreamKey);
        if ( pStreamingLocationLevelWithTempFailure != IStreamingLocationLevelPtr() )
        {
            pStreamingLocationLevel = pStreamingLocationLevelWithTempFailure;
            resetTempFailures();
        }
    }

    if ( pStreamingLocationLevel == IStreamingLocationLevelPtr() )
    {
        DEBUGLOG(LocationSelectorLog, "AS_DOMAIN_SELECTION_FAILURE_NO_AVAILABLE_LOCATION");
        return AS_DOMAIN_SELECTION_FAILURE_NO_AVAILABLE_LOCATION;
    }
    else
    {
        AseErrorCode retCode =
            pStreamingLocationLevel->getPreferredLocationSet(pStreamingLocationSet, requestSource);
        if ( (retCode == AS_NO_ERROR) &&
                (pStreamingLocationLevel->getLevel() != mCurrentStreamingLocationLevel->getLevel()) &&
                !mIgnoreLevelChange )
        {
            ILocationSetMonitorPtr pLocationSetMonitor = pStreamingLocationSet->getLocationSetMonitor();
            INetworkMonitor::PredictionSource source;
            double confidence;
            uint32_t locationThroughput = mNetworkMonitorPtr->
                                            getPredictedThroughput( pLocationSetMonitor,
                                                                    0,
                                                                    source,
                                                                    confidence );
            uint32_t avgConnectTime, varConnectTime, numberOfConnectionFailures;
            pLocationSetMonitor->connectionStat(avgConnectTime, varConnectTime, numberOfConnectionFailures);
            notifyLocationSelected( pStreamingLocationSet->getLocationSetKey(),
                                    pStreamingLocationSet->getDownloadableUrl(),
                                    varConnectTime, locationThroughput,
                                    "dlfail", "nextdomain", 0 );
            pLocationSelector->serverSelected(IStreamingReporter::SERVER_LOCATIONFAILOVER);
        }
        mCurrentStreamingLocationLevel = pStreamingLocationLevel;
        mIgnoreLevelChange = false;
        return retCode;
    }
}

void ServingStream::resetTempFailures()
{
    DEBUGLOG(LocationSelectorLog, "ServingStream::resetTempFailures");
    for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
            iter != mStreamingLocationLevelList.end();
            ++iter )
    {
        (*iter)->resetTempFailures();
    }
    return;
}

void ServingStream::reportTempNetworkFailure(AseUrl::HostPort const& destination)
{
    for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
            iter != mStreamingLocationLevelList.end();
            ++iter )
    {
        (*iter)->reportTempNetworkFailure(destination);
    }
}

void ServingStream::setFastplay(bool fastplay)
{
    for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
            iter != mStreamingLocationLevelList.end();
            ++iter )
    {
        (*iter)->setFastplay(fastplay);
    }
    mIgnoreLevelChange = true;
    return;
}

void ServingStream::enableLowGradeLevel()
{
    if ( mStreamingLocationLevelList.size() > 1 )
    {
        for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
                iter != mStreamingLocationLevelList.end();
                ++iter )
        {
            (*iter)->enableLowGradeLevel();
        }
    }
    mIgnoreLevelChange = true;
    return;
}

void ServingStream::disableLowGradeLevel()
{
    if ( mStreamingLocationLevelList.size() > 1 )
    {
        for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
                iter != mStreamingLocationLevelList.end();
                ++iter )
        {
            (*iter)->disableLowGradeLevel();
        }
    }
    mIgnoreLevelChange = true;
    return;
}

ManifestServingLocationPtr ServingStream::getManifestLocation( std::string const& locationKey,
                                    std::vector<ManifestServingLocationPtr> const& servingLocations)
{
    ManifestServingLocationPtr pServingLocation = ManifestServingLocationPtr();
    for ( vector<ManifestServingLocationPtr>::const_iterator iter = servingLocations.begin();
            iter != servingLocations.end();
            ++iter )
    {
        if ( (*iter)->getLocationKey() == locationKey )
        {
            pServingLocation = *iter;
            break;
        }
    }
    assert(pServingLocation != ManifestServingLocationPtr());
    return pServingLocation;
}


void ServingStream::constructLocationLevel( ManifestUrls const& manifestUrls,
                                            std::set<ManifestServingLocationPtr, DereferenceLess> servingLocations,
                                            uint32_t level, bool lowGradeFlag )
{
    DEBUGLOG(LocationSelectorLog, "stream: %s, level: %u, # of locations: %zu",
                                   mStreamKey.c_str(),
                                   level, servingLocations.size());
    IStreamingLocationLevelPtr pStreamingLocationLevel;

#if 0
    // disable multiple location streaming
    if ( mMultiLocationStreaming )
    {
        // multiple location streaming
        pStreamingLocationLevel.reset(new
                MultiStreamingLocationLevel(mLocationSelectorPtr.lock(), shared_from_this(),
                                            manifestUrls, servingLocations,
                                            mNetworkMonitorPtr, mConfigParamterPtr,
                                            level, lowGradeFlag) );
        updateLocationLevelList(lowGradeFlag, pStreamingLocationLevel);
    }
    else
#endif
    {
        switch ( mCacheSelectionAlgorithmFlavor )
        {
            case 0:
                // Original cache selection algorithm used by Nrdlib 12.3 and prior
                pStreamingLocationLevel.reset(new
                        OrigStreamingLocationLevel(mLocationSelectorPtr.lock(),
                                                   shared_from_this(),
                                                   manifestUrls, servingLocations,
                                                   mNetworkMonitorPtr, mConfigParamterPtr,
                                                   level, lowGradeFlag) );
                updateLocationLevelList(lowGradeFlag, pStreamingLocationLevel);
                break;
            case 1:
                // Contiuous cache selection algorithm
                pStreamingLocationLevel.reset(new
                        ContStreamingLocationLevel(mLocationSelectorPtr.lock(),
                                                   shared_from_this(),
                                                   manifestUrls, servingLocations,
                                                   mNetworkMonitorPtr, mConfigParamterPtr,
                                                   level, lowGradeFlag) );
                updateLocationLevelList(lowGradeFlag, pStreamingLocationLevel);
                break;
            default:
                pStreamingLocationLevel.reset(new
                        OrigStreamingLocationLevel(mLocationSelectorPtr.lock(),
                                                   shared_from_this(),
                                                   manifestUrls, servingLocations,
                                                   mNetworkMonitorPtr, mConfigParamterPtr,
                                                   level, lowGradeFlag) );
                updateLocationLevelList(lowGradeFlag, pStreamingLocationLevel);
                break;
        }
    }

    return;
}

void ServingStream::updateLocationLevelList(bool lowGradeFlag, IStreamingLocationLevelPtr pStreamingLocationLevel)
{
    if (lowGradeFlag)
    {
        pStreamingLocationLevel->setLevel(0);
        mStreamingLocationLevelList.push_front(pStreamingLocationLevel);
    }
    else
        mStreamingLocationLevelList.push_back(pStreamingLocationLevel);
    return;
}

void ServingStream::reportLocationFailure(std::string const& locationKey, IStreamingLocationLevel::STATUS status)
{
    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if ( pLocationSelector )
    {
        pLocationSelector->reportLocationFailure(mStreamKey, locationKey, status);
        mLocationSelectionSeqNo = pLocationSelector->locationSelectionSeqNo();
        DEBUGLOG(LocationSelectorLog,
                "reportLocationFailure: stream key: %s, seq no: %u",
                mStreamKey.c_str(), mLocationSelectionSeqNo);
    }

    return;
}

void ServingStream::notifyLocationSelected( std::string const& selLocatinoKey,
                                      ConstManifestUrlPtr pManifestUrl,
                                      uint32_t selLocationRtt,
                                      uint32_t selLocationBw,
                                      std::string const& testreason,
                                      std::string const& selreason,
                                      uint32_t fastselthreshold )
{

    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if ( pLocationSelector )
    {
        uint32_t selcdnid = pManifestUrl->getCdnId();
        ManifestServerPtr pManifestServer = mServers[selcdnid];
        string selcdnname = pManifestServer->getName();
        pLocationSelector->notifyLocationSelected(selLocatinoKey,
                selcdnid, selcdnname,
                selLocationRtt, selLocationBw,
                testreason, selreason, fastselthreshold);
        mLocationSelectionSeqNo = pLocationSelector->locationSelectionSeqNo();
        DEBUGLOG(LocationSelectorLog,
                "notifyLocationSelected: stream key: %s, location: %s, sel cdn id: %u, "
                "sel reason: %s, test reason: %s, seq no: %u",
                selLocatinoKey.c_str(),
                mStreamKey.c_str(),
                selcdnid,
                selreason.c_str(), testreason.c_str(),
                mLocationSelectionSeqNo);
    }

}


shared_ptr<LocationRecord> ServingStream::obtainLocationRecord(std::string const& locationKey)
{
    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if (pLocationSelector )
        return pLocationSelector->obtainLocationRecord(locationKey);
    else
        return shared_ptr<LocationRecord>();
}

void ServingStream::reportStreamingServerFailure(uint32_t cdnId)
{
    SimpleLocationSelectorPtr pLocationSelector = mLocationSelectorPtr.lock();
    if ( pLocationSelector )
        pLocationSelector->reportStreamingServerFailure(cdnId);
}

void ServingStream::obtainErrorReason(AseErrorCode& networkErrorCode, int32_t& httpStatusCode)
{
    networkErrorCode = mLastNetworkErrorCode;
    httpStatusCode = mLastHttpStatusCode;
}

void ServingStream::reportHttpError(int32_t errorCode, int32_t httpStatusCode)
{
    mLastHttpStatusCode = httpStatusCode;
    mLastNetworkErrorCode = errorCode;
}

void ServingStream::consumerStateChange()
{
    for ( list<IStreamingLocationLevelPtr>::iterator iter = mStreamingLocationLevelList.begin();
            iter != mStreamingLocationLevelList.end();
            ++iter )
    {
        (*iter)->consumerStateChange();
    }
}
