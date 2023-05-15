/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SingleStreamingLocationSet.h"
#include "SimpleLocationSelector.h"
#include "ServingUrl.h"
#include "ServingStream.h"

#include <nrdase/ILocationSetMonitor.h>
#include <nrdase/INetworkMonitor.h>


#include <vector>

using namespace netflix;
using namespace netflix::ase;
using namespace std;

AseErrorCode SingleStreamingLocationSet::construct(
                                            shared_ptr<ServingStream>               pServingStream,
                                            map<string, shared_ptr<CdnParam> >      cdnParamMap,
                                            ManifestUrls manifestUrls,
                                            ManifestServingLocationPtr pServingLocation,
                                            INetworkMonitorPtr pNetworkMonitor,
                                            bool lowGradeFlag,
                                            IAseConfigParameterPtr pConfigParameter,
                                            SingleStreamingLocationSetPtr& pLocationSet)
{
    pLocationSet.reset( new SingleStreamingLocationSet( pServingStream,
                                                        cdnParamMap,
                                                        pServingLocation,
                                                        pNetworkMonitor,
                                                        lowGradeFlag,
                                                        pConfigParameter) );
    pLocationSet->initialize(pServingStream, manifestUrls);
    return AS_NO_ERROR;
}


SingleStreamingLocationSet::SingleStreamingLocationSet(
                                            shared_ptr<ServingStream>               pServingStream,
                                            map<string, shared_ptr<CdnParam> >      cdnParamMap,
                                            ManifestServingLocationPtr pServingLocation,
                                            INetworkMonitorPtr pNetworkMonitor,
                                            bool lowGradeFlag,
                                            IAseConfigParameterPtr pConfigParameter ):
                                    mServingStreamPtr(pServingStream),
                                    mCdnParamMap(cdnParamMap),
                                    mKey(pServingLocation->getLocationKey()),
                                    mStreamKey(pServingStream->streamKey()),
                                    mRank(pServingLocation->getRank()),
                                    mWeight(pServingLocation->getWeight()),
                                    mLowGradeFlag(lowGradeFlag),
                                    mHttpConnectTimeout(0),
                                    mHttpStallTimeout(0),
                                    mStatus(IStreamingLocationLevel::INIT_STATE),
                                    mNetworkMonitorPtr(pNetworkMonitor),
                                    mConfigParameterPtr(pConfigParameter),
                                    mIndexForCurrentUrl(0),
                                    mHasResetFailure(false)
{
}

void SingleStreamingLocationSet::initialize(
        shared_ptr<ServingStream> pServingStream,
        ManifestUrls manifestUrls)
{
    // construct the serving urls
    mServingUrls.clear();
    ServingUrlPtr pServingUrl;
    /**
    DEBUGLOG(LocationSelectorLog,
                "tracking: SingleStreamingLocationSet: location key: %s, stream key: %s",
                mKey.c_str(), mStreamKey.c_str()); */
    map<uint32_t, shared_ptr<ManifestServer> > manifestServers = pServingStream->manifestServers();

    for ( vector<ConstManifestUrlPtr>::const_iterator iter = manifestUrls.begin();
            iter != manifestUrls.end();
            ++iter )
    {
        uint32_t cdnId = (*iter)->getCdnId();
        string cdnType = manifestServers[cdnId]->getCdnType();

        shared_ptr<CdnParam> pCdnParam = shared_ptr<CdnParam>();
        if ( mCdnParamMap.find(cdnType) != mCdnParamMap.end() )
        {
            pCdnParam = mCdnParamMap[cdnType];
            mHttpConnectTimeout = pCdnParam->mHttpConnectTimeout;
            mHttpStallTimeout = pCdnParam->mHttpStallTimeout;
        }
        pServingUrl.reset( new ServingUrl(
                shared_from_this(),
                *iter, pCdnParam, mConfigParameterPtr) );
        mServingUrls.push_back(pServingUrl);
    }
}

IStreamingLocationLevel::STATUS
SingleStreamingLocationSet::getStatus()
{
    checkLocation();
    return mStatus;
}

uint32_t SingleStreamingLocationSet::getRank() const
{
    return mRank;
}

uint32_t SingleStreamingLocationSet::getWeight() const
{
    return mWeight;
}

std::string const& SingleStreamingLocationSet::getLocationSetKey() const
{
    return mKey;
}


std::string SingleStreamingLocationSet::getStreamKey()
{
    return mStreamKey;
}

void SingleStreamingLocationSet::getTimeouts(uint32_t& httpConnectTimeout, uint32_t& httpStallTimeout) const
{
    httpConnectTimeout = mHttpConnectTimeout;
    httpStallTimeout = mHttpStallTimeout;
}

bool SingleStreamingLocationSet::lowGrade() const
{
    return mLowGradeFlag;
}

bool SingleStreamingLocationSet::compare(IStreamingLocationSetPtr pStreamingLocationSet)
{
    if ( pStreamingLocationSet.get() == NULL )
        return false;

    if ( (mKey == pStreamingLocationSet->getLocationSetKey()) &&
            (mStreamKey == pStreamingLocationSet->getStreamKey()) )
        return true;
    else
        return false;
}

ILocationSetMonitorPtr SingleStreamingLocationSet::getLocationSetMonitor()
{
    return mNetworkMonitorPtr->getLocationSetMonitor(mKey);
}

ConstManifestUrlPtr SingleStreamingLocationSet::getDownloadableUrl() const
{
    return mServingUrls[mIndexForCurrentUrl]->getManifestUrl();
}

uint32_t SingleStreamingLocationSet::getPrimaryCdnId() const
{
    return mServingUrls[mIndexForCurrentUrl]->getManifestUrl()->getCdnId();
}

/** Report HTTP error for a location. */
int32_t SingleStreamingLocationSet::reportHttpError(
                        ConstManifestUrlPtr pManifestUrl,
                        int32_t errorCode,
                        int32_t httpStatusCode )
{
    DEBUGLOG(LocationSelectorLog,"reportHttpError: %s, %d, %d",
        pManifestUrl->getUrl().str().c_str(),
        errorCode,
        httpStatusCode );

    ServingUrlPtr pCurrServingUrl = mServingUrls[mIndexForCurrentUrl];
    IStreamingLocationLevel::STATUS urlStatus = IStreamingLocationLevel::INIT_STATE;
    if ( !pManifestUrl || (pCurrServingUrl->str() == pManifestUrl->getUrl().str()) )
    {
        pCurrServingUrl->reportHttpError(errorCode, httpStatusCode);
        urlStatus = pCurrServingUrl->getStatus();
        locationSetHealthCheck();
    }

    ServingStreamPtr pServingStream = mServingStreamPtr.lock();
    if ( pServingStream )
    {
        if ( (urlStatus == IStreamingLocationLevel::PERM_FAILED_STATE) ||
                (urlStatus == IStreamingLocationLevel::TEMP_FAILED_STATE) )
        {
            uint32_t cdnId = pCurrServingUrl->getManifestUrl()->getCdnId();
            pServingStream->reportStreamingServerFailure(cdnId);
        }
        pServingStream->reportHttpError(errorCode, httpStatusCode);
    }

    return AS_NO_ERROR;
}

void SingleStreamingLocationSet::resetTempFailures()
{
    if ( mStatus == IStreamingLocationLevel::TEMP_FAILED_STATE )
    {
        DEBUGLOG(LocationSelectorLog,
                "reset failure: location key: %s",
                mKey.c_str());
        mIndexForCurrentUrl = locationSetHealthCheck();
        for ( vector<ServingUrlPtr>::iterator iter = mServingUrls.begin();
                iter != mServingUrls.end();
                ++iter )
        {
            (*iter)->resetTempFailures();
        }
        mStatus = IStreamingLocationLevel::INIT_STATE;
        mHasResetFailure = true;
    }
    return;
}

void SingleStreamingLocationSet::reportTempNetworkFailure(AseUrl::HostPort const& destination)
{
    for ( vector<ServingUrlPtr>::iterator iter = mServingUrls.begin();
            iter != mServingUrls.end();
            ++iter )
    {
        if ( (*iter)->compare(destination) )
        {
            (*iter)->reportTempNetworkFailure();
            locationSetHealthCheck();
            break;
        }
    }
    return;
}

uint32_t SingleStreamingLocationSet::locationSetHealthCheck()
{
    IStreamingLocationLevel::STATUS status = mServingUrls[mIndexForCurrentUrl]->getStatus();
    if ( (status != IStreamingLocationLevel::TEMP_FAILED_STATE) &&
            (status != IStreamingLocationLevel::PERM_FAILED_STATE) )
        return mIndexForCurrentUrl;

    uint32_t numOfUrls = mServingUrls.size();
    uint32_t probingIndex = (mIndexForCurrentUrl + 1) % numOfUrls;
    int32_t indexForNextValidUrl = -1;

    while ( probingIndex != mIndexForCurrentUrl )
    {
        status = mServingUrls[probingIndex]->getStatus();
        DEBUGLOG(LocationSelectorLog,
                "probing index: %u, current url index: %u, status: %u",
                probingIndex, mIndexForCurrentUrl, status);
        if ( (status != IStreamingLocationLevel::TEMP_FAILED_STATE)
                && (status != IStreamingLocationLevel::PERM_FAILED_STATE) )
        {
            indexForNextValidUrl = probingIndex;
            break;
        }
        else if ( (status == IStreamingLocationLevel::TEMP_FAILED_STATE) &&
                (indexForNextValidUrl < 0))
        {
            indexForNextValidUrl = probingIndex;
        }

        probingIndex = (probingIndex + 1) % numOfUrls;
    }

    if ( indexForNextValidUrl < 0)
        indexForNextValidUrl = (mIndexForCurrentUrl + 1) % numOfUrls;

    mStatus = mServingUrls[indexForNextValidUrl]->getStatus();
    mIndexForCurrentUrl = indexForNextValidUrl;

    if ( (mStatus == IStreamingLocationLevel::TEMP_FAILED_STATE) ||
            (mStatus == IStreamingLocationLevel::PERM_FAILED_STATE) )
    {
        ServingStreamPtr pServingStream = mServingStreamPtr.lock();
        if ( pServingStream )
        {
            pServingStream->reportLocationFailure(mKey, mStatus);
        }
        DEBUGLOG(LocationSelectorLog, "locationSetHealthCheck: location set: %s, failed state: %u",
                            mKey.c_str(), mStatus);
    }

    return indexForNextValidUrl;
}

void SingleStreamingLocationSet::checkLocation()
{
    ServingStreamPtr pServingStream = mServingStreamPtr.lock();
    if ( pServingStream )
    {
        shared_ptr<LocationRecord> locationRecord = pServingStream->obtainLocationRecord(mKey);
        if ( locationRecord->mNetworkFailure && !mHasResetFailure && (mStatus != IStreamingLocationLevel::PERM_FAILED_STATE) )
        {
            mStatus = IStreamingLocationLevel::TEMP_FAILED_STATE;
        }
    }
    return;
}
