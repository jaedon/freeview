/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STREAMING_LOCATION_LEVEL_H
#define STREAMING_LOCATION_LEVEL_H

#include <nrdase/IAseConfigParameter.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdase/Manifest.h>
#include <nrdase/IMediaConsumer.h>
#include <nrdase/IStreamingReporter.h>
#include <nrdnet/AseUrl.h>

#include "IStreamingLocationLevel.h"
#include <set>

namespace netflix {
namespace ase {

class IStreamingLocationSet;
class SingleStreamingLocationSet;
class SimpleLocationSelector;
class INetworkMonitor;
class ServingStream;
class ServingUrl;

/**
 * @class StreamingLocationLevel
 * @brief The class that implements IStreamingLocationLevel for single location streaming
 *        It has two subclasses:
 *        1) a subclass for the existing behaviour where location probing only
 *           takes place at startup and after rebuffer/seeks
 *        2) a subclass implementing continuous location monitoring, where preference order changes take place:
 *        * Based on performance comparisons within the same level;
 *        * Based on the need for performance data collection
 */
class StreamingLocationLevel : public IStreamingLocationLevel
{
public:
    typedef shared_ptr<StreamingLocationLevel> StreamingLocationLevelPtr;

    /** The reason the domain selector decides to start testing domains. */
    enum TestReason
    {
        TEST_PERFPROBE,
        TEST_STARTUP,
        TEST_REBUFFER,
        TEST_SEEK,
        TEST_DLFAIL
    };

    /** The reason the domain selector selects a domain. */
    enum SelectReason
    {
        SEL_MAXWEIGHTED_BW,
        SEL_FAST_SELECTION,
        SEL_REUSE_PREVIOUS,
        SEL_NEXT_DOMAIN,
        SEL_ONLY_CHOICE,
        SEL_MCQUEEN,
        SEL_TO_LOW_GRADE,
        SEL_FROM_LOW_GRADE
    };

    StreamingLocationLevel(shared_ptr<SimpleLocationSelector> pLocationSelector,
                           shared_ptr<ServingStream>         pServingStream,
                           IAseConfigParameterPtr pConfigParameter,
                           uint32_t level, bool lowGradeFlag);


    virtual AseErrorCode getPreferredLocationSet(shared_ptr<IStreamingLocationSet>& pStreamingLocationSet,
                                                 ILocationSelector::LocationSelectionRequestSource requestSource);
    virtual STATUS getStatus();
    virtual void checkSelection();
    virtual void consumerStateChange();

    // reset the state out of the temp failure mode
    virtual void resetTempFailures();
    virtual void setFastplay(bool fastplay);
    virtual void setLevel(uint32_t level);
    virtual uint32_t getLevel() const;

    /**
     * enable/disable the level with low grade
     */
    virtual void enableLowGradeLevel();
    virtual void disableLowGradeLevel();

    void notifyLocationSelected(uint32_t locationSetIndex, uint32_t fastselthreshold);

    virtual void reportTempNetworkFailure(AseUrl::HostPort const& destination);

    virtual ~StreamingLocationLevel() {}

protected:
    weak_ptr<SimpleLocationSelector> mLocationSelectorPtr;
    weak_ptr<ServingStream>          mServingStreamPtr;
    //The pointer to the configuration parameter.
    IAseConfigParameterPtr mConfigParameterPtr;

    uint32_t mLevel; // a level reflecting cost of the location
    bool mLowGradeFlag;

    bool mFastplay;
    bool mConsumerStateChange;
    bool mFreezeSwitch;

    STATUS mPrevStatus;
    STATUS mStatus;

    // index for the current location set in mStreamingLocationSetList
    uint32_t mCurrentLocationSetIndex;
    uint32_t mPrevLocationSetIndex;
    uint32_t mPrevCdnId;
    IStreamingReporter::ServerSelectionReason mRecentServerSelReason;
    uint32_t mProbingIndex; // current probing index
    uint32_t mSelectedIndex; // selected index
    std::vector< shared_ptr<SingleStreamingLocationSet> > mStreamingLocationSetList;

    double   mThroughputLowConfidenceLevel;
    double   mThroughputHighConfidenceLevel;

    uint32_t mThroughputMeasureWindow;

    std::map<std::string, shared_ptr<CdnParam> > mCdnParamMap;

    //The reason to start testing domains.
    TestReason mTestReason;

    //The reason a domain is selected.
    SelectReason mSelReason;

    IMediaConsumer::State mPreviousMediaConsumerState;
    IMediaConsumer::State mCurrentMediaConsumerState;

    shared_ptr<INetworkMonitor> mNetworkMonitorPtr;

// protected methods
protected:
    // driven by network error reporting
    uint32_t locationLevelHealthCheck();

    void initializeLocationLevel(ManifestUrls const& manifestUrls,
                                 std::set<ManifestServingLocationPtr, DereferenceLess> const& servingLocations,
                                 shared_ptr<INetworkMonitor> pNetworkMonitor);

    /**
     * check if a new server is selected. If so, notify IStreamingReporter with the correct reason.
     */
    void checkNewServerSelected(shared_ptr<IStreamingLocationSet> pSelStreamingLocationSet);
};

typedef StreamingLocationLevel::StreamingLocationLevelPtr StreamingLocationLevelPtr;

inline
void StreamingLocationLevel::setLevel(uint32_t level)
{
    mLevel = level;
}

inline
uint32_t StreamingLocationLevel::getLevel() const
{
    return mLevel;
}

}}

#endif
