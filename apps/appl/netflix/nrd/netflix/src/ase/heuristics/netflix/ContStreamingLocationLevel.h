/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef CONT_STREAMING_LOCATION_LEVEL_H
#define CONT_STREAMING_LOCATION_LEVEL_H

#include "StreamingLocationLevel.h"
#include <nrdase/Manifest.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/INetworkMonitor.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>

namespace netflix {
namespace ase {

class SimpleLocationSelector;
class INetworkMonitor;
class IStreamingLocationSet;
class ServingStream;


/**
 * @class ContStreamingLocationLevel
 * @brief
 *        2) a subclass implementing continuous location monitoring, where preference order changes take place:
 *        * Based on performance comparisons within the same level;
 *        * Based on the need for performance data collection
 */
class ContStreamingLocationLevel : public StreamingLocationLevel
{
public:
    typedef shared_ptr<ContStreamingLocationLevel> ContStreamingLocationLevelPtr;
    /** constructor
     * @param[in] manifestUrls: all manifest urls belong to this location level
     * @param[in] servingLocations: all serving locations belong to this locatino level
     * @param[in] pConfigParameter
     * @param[in] level
     * @param[in] lowGradeFlag
     */
    ContStreamingLocationLevel( shared_ptr<SimpleLocationSelector> pLocationSelector,
                                shared_ptr<ServingStream> pServingStream,
                                ManifestUrls const& manifestUrls,
                                std::set<ManifestServingLocationPtr, DereferenceLess> servingLocations,
                                shared_ptr<INetworkMonitor> pNetworkMonitor,
                                IAseConfigParameterPtr pConfigParameter,
                                uint32_t level, bool lowGradeFlag );

    virtual AseErrorCode getPreferredLocationSet(shared_ptr<IStreamingLocationSet>& pStreamingLocationSet,
                                                 ILocationSelector::LocationSelectionRequestSource requestSource);

    virtual STATUS getStatus();
    virtual void checkSelection();

protected:
    // config parameters
    uint32_t mThroughputProbingEnterThreshold;
    uint32_t mThroughputProbingExitThreshold;

    uint32_t mLocationProbingTimeout;
    uint32_t mLocationSelectionCheckTimeout;

    uint32_t mFinalLocationSelectionBwThreshold;

    AseTimeVal mProbingStartTime;
    AseTimeVal mPrevCheckTime;

protected:
    void updateLocationMonitoring();

    /**
     * @param[out] foundIndex: true if a valid location is found
     * @return: the next valide location index for probing
     */
    uint32_t getNextProbingIndex(bool& foundIndex);

    /**
     * judge whether the provided location is eligible for probing
     */
    bool eligibleForProbing(uint32_t index);

    /**
     * judge whether the probing for the provided location is completed.
     */
    bool completeProbing(uint32_t index);

    /**
     * updated the cache selection result with the new probing result
     */
    void updateCacheSelection(uint32_t probingIndex);

    uint32_t calculateWeightedThroughput(uint32_t locationSetIndex, double& confidence,
                                         INetworkMonitor::PredictionSource& predictionSource);

    uint32_t calculateThroughput(uint32_t locationSetIndex, double& confidence,
                                 INetworkMonitor::PredictionSource& predictionSource);

    // select a location based on performance
    void selectLocationByPerformance();
    void initialLocationSelection();

};

typedef ContStreamingLocationLevel::ContStreamingLocationLevelPtr ContStreamingLocationLevelPtr;


}}

#endif
