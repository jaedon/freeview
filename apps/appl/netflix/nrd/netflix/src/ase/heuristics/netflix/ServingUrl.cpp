/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "ServingUrl.h"
#include "SimpleLocationSelector.h"
#include "IStreamingLocationSet.h"

using namespace netflix;
using namespace netflix::ase;
using namespace std;

ServingUrl::ServingUrl(shared_ptr<IStreamingLocationSet> pLocationSet,
                       ConstManifestUrlPtr pManifestUrl, shared_ptr<CdnParam> pCdnParam,
                       IAseConfigParameterPtr pConfigParameter)
                    : mStatus(StreamingLocationLevel::SELECTED_STATE), mConfigParameterPtr(pConfigParameter),
                      mManifestUrlPtr(pManifestUrl),
                      mLocationSetPtr(pLocationSet)
{
    NRDP_UNUSED(pCdnParam);
    mStr = pManifestUrl->getUrl().str();
}

int32_t ServingUrl::reportTempNetworkFailure()
{
    mStatus = StreamingLocationLevel::TEMP_FAILED_STATE;
    DEBUGLOG(LocationSelectorLog, "Temp network failure at: %s", mStr.c_str());
    return AS_NO_ERROR;
}

bool ServingUrl::compare(AseUrl::HostPort const& destination)
{
    AseUrl::HostPort myHostPort = mManifestUrlPtr->getUrl().GetHostPort();
    return ( (myHostPort.first == destination.first) &&
                (myHostPort.second == destination.second) );
}


int32_t ServingUrl::reportHttpError(
            int32_t errorCode,
            int32_t httpStatusCode)
{
    NRDP_UNUSED(errorCode);      // suppress unused variable warning
    NRDP_UNUSED(httpStatusCode); // suppress unused variable warning
    DEBUGLOG(LocationSelectorLog, "reportHttpError, url: %s, status %d, code: %d",
                    mStr.c_str(), errorCode, httpStatusCode);

    mStatus = StreamingLocationLevel::PERM_FAILED_STATE;
    return AS_NO_ERROR;
}

void ServingUrl::resetTempFailures()
{
    if ( mStatus != StreamingLocationLevel::PERM_FAILED_STATE )
    {
        mStatus = StreamingLocationLevel::SELECTED_STATE;
    }
}

