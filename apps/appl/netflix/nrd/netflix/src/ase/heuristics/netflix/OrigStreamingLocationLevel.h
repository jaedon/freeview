/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ORIG_STREAMING_LOCATION_LEVEL_H
#define ORIG_STREAMING_LOCATION_LEVEL_H

#include "StreamingLocationLevel.h"
#include <nrdase/Manifest.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdbase/AseErrorCodes.h>

#include <set>
namespace netflix {
namespace ase {

class SimpleLocationSelector;
class INetworkMonitor;
class IStreamingLocationSet;
/**
 * @class OrigStreamingLocationLevel
 * @brief: Implement the original locatino selection logic, which has been used in Nrdlib 12.2 and prior.
 *         The location throughput probing only happens at startup and after rebuffer/seeks
 */
class OrigStreamingLocationLevel : public StreamingLocationLevel
{
public:
    typedef shared_ptr<OrigStreamingLocationLevel> OrigStreamingLocationLevelPtr;

    /** constructor
     * @param[in] manifestUrls: all manifest urls belong to this location level
     * @param[in] servingLocations: all serving locations belong to this locatino level
     * @param[in] pConfigParameter
     * @param[in] level
     * @param[in] lowGradeFlag
     */
     OrigStreamingLocationLevel(shared_ptr<SimpleLocationSelector> pLocationSelector,
                                shared_ptr<ServingStream>      pServingStream,
                                ManifestUrls const& manifestUrls,
                                std::set<ManifestServingLocationPtr, DereferenceLess> servingLocations,
                                shared_ptr<INetworkMonitor> pNetworkMonitor,
                                IAseConfigParameterPtr pConfigParameter,
                                uint32_t level, bool lowGradeFlag);

    virtual AseErrorCode getPreferredLocationSet(shared_ptr<IStreamingLocationSet>& pStreamingLocationSet,
                                                 ILocationSelector::LocationSelectionRequestSource requestSource);

    virtual STATUS getStatus();

    virtual void checkSelection();
protected:
    // driven by network data arrival
    void updateLocationLevel(bool forceSelection, bool enableLogging);

    uint32_t foundMatchedLocation(std::string const& locationSetKey);

    uint32_t mFastDomainSelectionBwThreshold;       // kbps
    uint32_t mLocationSelectionSeqNo;
};

typedef OrigStreamingLocationLevel::OrigStreamingLocationLevelPtr OrigStreamingLocationLevelPtr;
}}
#endif
