/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SERVING_URL_H
#define SERVING_URL_H

#include <nrdase/Manifest.h>
#include <nrdase/IAseConfigParameter.h>

#include <nrdnet/NetCommonDataTypes.h>
#include <nrdnet/AseUrl.h>

#include "StreamingLocationLevel.h"


#include <string>


namespace netflix {
namespace ase {

class IStreamingLocationSet;

class ServingUrl
{
public:
    typedef shared_ptr<ServingUrl> ServingUrlPtr;

    ServingUrl(shared_ptr<IStreamingLocationSet> pLocationSet, ConstManifestUrlPtr pManifestUrl, shared_ptr<CdnParam> pCdnParam, IAseConfigParameterPtr pConfigParameter);

    const std::string& str() const;

    ConstManifestUrlPtr getManifestUrl() const;

    IStreamingLocationLevel::STATUS getStatus() const;

    int32_t reportTempNetworkFailure();
    bool compare(AseUrl::HostPort const& destination);

    /** Report HTTP error (4XX, 5XX) for a location. */
    int32_t reportHttpError(
                        int32_t errorCode,
                        int32_t httpStatusCode );

    void resetTempFailures();

protected:
    /**
     * supported values:
     * SELECTED_STATE, TEMP_FAILED_STATE, PERM_FAILED_STATE
     */
    IStreamingLocationLevel::STATUS mStatus;

    IAseConfigParameterPtr mConfigParameterPtr;

    ConstManifestUrlPtr mManifestUrlPtr;

    weak_ptr<IStreamingLocationSet> mLocationSetPtr;

    std::string         mStr;
};

typedef ServingUrl::ServingUrlPtr ServingUrlPtr;

inline const std::string& ServingUrl::str() const
{
    return mStr;
}

inline IStreamingLocationLevel::STATUS ServingUrl::getStatus() const
{
    return mStatus;
}

inline ConstManifestUrlPtr ServingUrl::getManifestUrl() const
{
    return mManifestUrlPtr;
}

}}

#endif
