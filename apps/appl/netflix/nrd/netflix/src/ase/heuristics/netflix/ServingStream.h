/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SERVING_STREAM_H
#define SERVING_STREAM_H

#include <nrdase/IAseConfigParameter.h>
#include <nrdase/Manifest.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseUrl.h>
#include "IStreamingLocationLevel.h"

#include <string>
#include <list>
#include <vector>

namespace netflix {
namespace ase {

class IStreamingLocationSet;
class IStreamingLocationLevel;
class SimpleLocationSelector;
class INetworkMonitor;
class LocationRecord;
class ServingUrl;
/**
 * @class ServingStream
 */
class ServingStream : public enable_shared_from_this<ServingStream>
{
public:
    typedef shared_ptr<ServingStream> ServingStreamPtr;
    static AseErrorCode construct(
                  shared_ptr<SimpleLocationSelector> pLocationSelector,
                  MediaType mediaType,
                  ManifestStreamPtr pManifestStream,
                  std::vector<ManifestServingLocationPtr> const& servingLocations,
                  std::map<uint32_t, ManifestServerPtr> const& servers,
                  shared_ptr<INetworkMonitor> pNetworkMonitor,
                  IAseConfigParameterPtr pConfigParameter,
                  ServingStreamPtr& pServingStream);

    ~ServingStream();
    /**
     * get the preferred location set for this stream
     * @param[out] pStreamingLocationSet
     */
    AseErrorCode getPreferredLocationSet(shared_ptr<IStreamingLocationSet>& pStreamingLocationSet,
                                         ILocationSelector::LocationSelectionRequestSource requestSource );

    /**
     * enable/disable the level with low grade
     */
    void enableLowGradeLevel();
    void disableLowGradeLevel();

    /**
     * reset all levels out of temp_failed_state
     */
    void resetTempFailures();

    void reportLocationFailure(std::string const& locationKey, IStreamingLocationLevel::STATUS status);
    shared_ptr<LocationRecord> obtainLocationRecord(std::string const& locationKey);

    void notifyLocationSelected( std::string const& selLocatinoKey,
                           ConstManifestUrlPtr pManifestUrl,
                           uint32_t selLocationRtt,
                           uint32_t selLocationBw,
                           std::string const& testreason,
                           std::string const& selreason,
                           uint32_t fastselthreshold );

    std::string const& streamKey() const;
    MediaType mediaType() const;

    std::map<uint32_t, shared_ptr<ManifestServer> > const& manifestServers() const;

    void setFastplay(bool fastplay);

    void reportTcpFailure(AseErrorCode errorCode);
    void reportHttpError(int32_t errorCode, int32_t httpStatusCode);
    void reportStreamingServerFailure(uint32_t cdnId);
    void reportDataReceived();

    void obtainErrorReason(AseErrorCode& networkErrorCode, int32_t& httpStatusCode);

    void consumerStateChange();

    void initialize();

    void reportTempNetworkFailure(AseUrl::HostPort const& destination);

    shared_ptr<ManifestStream> manifestStream();

protected:
    weak_ptr<SimpleLocationSelector> mLocationSelectorPtr;

    MediaType                  mMediaType;
    shared_ptr<ManifestStream> mManifestStreamPtr;
    std::vector<shared_ptr<ManifestServingLocation> > mServingLocations;
    std::map<uint32_t, shared_ptr<ManifestServer> > mServers;

    std::string mStreamKey;

    std::list< shared_ptr<IStreamingLocationLevel> > mStreamingLocationLevelList;

    shared_ptr<IStreamingLocationLevel> mCurrentStreamingLocationLevel;
    bool mIgnoreLevelChange;

    shared_ptr<INetworkMonitor> mNetworkMonitorPtr;
    IAseConfigParameterPtr mConfigParamterPtr;

    bool mMultiLocationStreaming; // multiCdnSelector
    uint32_t mCacheSelectionAlgorithmFlavor; // cacheSelectionAlgorithmFlavor

    uint32_t mLocationSelectionSeqNo;

    AseErrorCode mLastNetworkErrorCode;
    int32_t mLastHttpStatusCode;
protected:
    ServingStream(shared_ptr<SimpleLocationSelector> pLocationSelector,
                  MediaType mediaType,
                  ManifestStreamPtr pManifestStream,
                  std::vector<ManifestServingLocationPtr> const& servingLocations,
                  std::map<uint32_t, ManifestServerPtr> const& servers,
                  shared_ptr<INetworkMonitor> pNetworkMonitor,
                  IAseConfigParameterPtr pConfigParameter);

    ManifestServingLocationPtr getManifestLocation( std::string const& locationKey,
                                    std::vector<ManifestServingLocationPtr> const& servingLocations);


    void constructLocationLevel( ManifestUrls const& manifestUrls,
                                 std::set<ManifestServingLocationPtr, DereferenceLess> servingLocations,
                                 uint32_t level, bool lowGradeFlag );

    void updateLocationLevelList(bool lowGradeFlag, shared_ptr<IStreamingLocationLevel> pStreamingLocationLevel);
};

typedef ServingStream::ServingStreamPtr ServingStreamPtr;

inline
std::string const& ServingStream::streamKey() const
{
    return mStreamKey;
}

inline
MediaType ServingStream::mediaType() const
{
    return mMediaType;
}

inline
std::map<uint32_t, shared_ptr<ManifestServer> > const& ServingStream::manifestServers() const
{
    return mServers;
}

inline shared_ptr<ManifestStream> ServingStream::manifestStream()
{
    return mManifestStreamPtr;
}

}}

#endif
