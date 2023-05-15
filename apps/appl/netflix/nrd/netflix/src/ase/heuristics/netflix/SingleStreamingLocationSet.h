/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STREAMING_LOCATION_SET_H
#define STREAMING_LOCATION_SET_H

#include "IStreamingLocationLevel.h"
#include "IStreamingLocationSet.h"
#include <nrdase/Manifest.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/AseCommonDataTypes.h>

#include <vector>

namespace netflix {
namespace ase {

class ServingUrl;
class ServingStream;
class INetworkMonitor;

/**
 * @class SingleStreamingLocationSet
 * @brief SingleStreamingLocationSet represents a set of locations for a given serving stream.
 *        It has two subclasses:
 *        * SingleLocation represents a single location
 *        * MultiLocation represents a particular combination of SingleLocations
 *        A StreamingLocationSet maintains or have access to network statistics and network errors.
 *        It controls local failover among ServingUrls.
 */
class SingleStreamingLocationSet : public IStreamingLocationSet
{
public:
    typedef shared_ptr<SingleStreamingLocationSet> SingleStreamingLocationSetPtr;

    static AseErrorCode construct(
                          shared_ptr<ServingStream> pServingStream,
                          std::map<std::string, shared_ptr<CdnParam> > cdnParamMap,
                          ManifestUrls manifestUrls,
                          ManifestServingLocationPtr pServingLocation,
                          shared_ptr<INetworkMonitor> pNetworkMonitor,
                          bool lowGradeFlag,
                          IAseConfigParameterPtr pConfigParameter,
                          shared_ptr<SingleStreamingLocationSet>& pLocationSet);

    IStreamingLocationLevel::STATUS getStatus();

    uint32_t getRank() const;
    uint32_t getWeight() const;
    virtual std::string const& getLocationSetKey() const;
    virtual std::string getStreamKey();

    virtual void getTimeouts(uint32_t& httpConnectTimeout, uint32_t& httpStallTimeout) const;

    virtual bool lowGrade() const;

    virtual shared_ptr<ILocationSetMonitor> getLocationSetMonitor();

    /* ---------------------------------------------------------------------------------*/
    /*                      actions                                                     */
    /* ---------------------------------------------------------------------------------*/
    virtual ConstManifestUrlPtr getDownloadableUrl() const;
    virtual uint32_t getPrimaryCdnId() const;

    void resetTempFailures();

    void reportTempNetworkFailure(AseUrl::HostPort const& destination);

    /* ---------------------------------------------------------------------------------*/
    /*                      report failures                                             */
    /* ---------------------------------------------------------------------------------*/
    /** Report HTTP error for a location. */
    virtual int32_t reportHttpError(
                        ConstManifestUrlPtr pManifestUrl,
                        int32_t errorCode,
                        int32_t httpStatusCode );

    virtual bool compare(IStreamingLocationSetPtr pStreamingLocationSet);

    virtual ~SingleStreamingLocationSet()
    {
    }
protected:
    weak_ptr<ServingStream> mServingStreamPtr;
    std::map<std::string, shared_ptr<CdnParam> > mCdnParamMap;
    std::string mKey;
    std::string mStreamKey;
    uint32_t mRank;
    uint32_t mWeight;

    bool mLowGradeFlag;

    uint32_t mHttpConnectTimeout;
    uint32_t mHttpStallTimeout;

    // supported values: INIT_STATE, TEMP_FAILED_STATE, PERM_FAILED_STATE
    IStreamingLocationLevel::STATUS mStatus;

    shared_ptr<INetworkMonitor> mNetworkMonitorPtr;
    IAseConfigParameterPtr mConfigParameterPtr;

    uint32_t mIndexForCurrentUrl;
    std::vector< shared_ptr<ServingUrl> > mServingUrls;

    bool     mHasResetFailure;

protected:
    // constructor
    SingleStreamingLocationSet(
                          shared_ptr<ServingStream> pServingStream,
                          std::map<std::string, shared_ptr<CdnParam> > cdnParamMap,
                          ManifestServingLocationPtr pServingLocation,
                          shared_ptr<INetworkMonitor> pNetworkMonitor,
                          bool lowGradeFlag,
                          IAseConfigParameterPtr pConfigParameter);

    uint32_t locationSetHealthCheck();
    void     checkLocation();
    void initialize(shared_ptr<ServingStream> pServingStream, ManifestUrls manifestUrls);
};

typedef SingleStreamingLocationSet::SingleStreamingLocationSetPtr SingleStreamingLocationSetPtr;

}}

#endif
