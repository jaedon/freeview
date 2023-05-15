/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_STREAMING_LOCATION_LEVEL_H
#define I_STREAMING_LOCATION_LEVEL_H

#include <nrdbase/AseErrorCodes.h>
#include <nrdase/Manifest.h>
#include <nrdase/ILocationSelector.h>

namespace netflix {
namespace ase {

class IStreamingLocationSet;
class ServingUrl;

/**
 * @class StreamingLocationLevel
 * @brief The class that implements the location selection logic.
 *        It has two subclasses:
 *        1) a subclass for the existing behaviour where location probing only
 *           takes place at startup and after rebuffer/seeks
 *        2) a subclass implementing continuous location monitoring, where preference order changes take place:
 *        * Based on performance comparisons within the same level;
 *        * Based on the need for performance data collection
 */
class IStreamingLocationLevel
{
public:
    typedef shared_ptr<IStreamingLocationLevel> IStreamingLocationLevelPtr;

    enum STATUS
    {
        INIT_STATE,
        MEASURING_STATE,
        SELECTED_STATE,
        NOT_USABLE_STATE, // not available due to low grade flag
        TEMP_FAILED_STATE, // temporary failure
        PERM_FAILED_STATE // permanent failure
    };

    virtual AseErrorCode getPreferredLocationSet(
                        shared_ptr<IStreamingLocationSet>& pStreamingLocationSet,
                        ILocationSelector::LocationSelectionRequestSource requestSource) = 0;
    virtual STATUS getStatus() = 0;

    /**
     * check the location selection when location levels from other serving streams have one of the followings
     * 1. two locations change order;
     * 2. one location is failed due to network failure
     */
    virtual void checkSelection() = 0;

    // reset the state out of the temp failure mode
    virtual void resetTempFailures() = 0;

    virtual void setLevel(uint32_t level) = 0;

    virtual uint32_t getLevel() const = 0;

    virtual void consumerStateChange() = 0;

    /**
     * enable/disable the level with low grade
     */
    virtual void enableLowGradeLevel() = 0;
    virtual void disableLowGradeLevel() = 0;

    virtual void setFastplay(bool fastplay) = 0;

    virtual void reportTempNetworkFailure(AseUrl::HostPort const& destination) = 0;

    virtual ~IStreamingLocationLevel() {}

};

typedef IStreamingLocationLevel::IStreamingLocationLevelPtr IStreamingLocationLevelPtr;

}}

#endif
