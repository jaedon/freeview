/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef LOCATION_SET_MONITOR_H
#define LOCATION_SET_MONITOR_H

#include <nrdase/ILocationSetMonitor.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/AverageThroughput.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/INetworkMonitor.h>
#include <nrdase/DebugLogging.h>

#include <string>
#include <deque>

namespace netflix {
namespace ase {

DECLARE_LOGGER( LocationSetMonitorLog );

class LocationSetMonitorAggregate;
class ThroughputHistory;

/**
 * @class LocationSetMonitor
 * @brief A simple location set monitor, which uses run-time average to estimate the location's throughput
 */
class LocationSetMonitor : public ILocationSetMonitor
{
public:
    typedef LocationSetMonitorAggregate AggregateType;

    typedef shared_ptr<LocationSetMonitor> LocationSetMonitorPtr;

    struct Params
    {
        std::map<uint32_t,uint32_t> mThroughputHalfLives;

        AseTimeVal mThroughputWarmupTime;
        AseTimeVal mMinimumPauseTime;
        AseTimeVal mConnectionFailureExpirationTime;
        AseTimeVal mShortTermThreshold;
        AseTimeVal mDistributionUpdateInterval;
        AseTimeVal mThroughputCalculationInterval;
        AseTimeVal mHistoryPredictionInterval;
        AseTimeVal mLockPeriodAfterProbeComplete;

        uint32_t        mMinimumMeasurementTime;
        uint32_t        mMinimumMeasurementBytes;

        bool            mFastWarmup;

        void initialize( IAseConfigParameterPtr pConfigParameters );
    };


    static LocationSetMonitorPtr create(    std::string const&              key,
                                            IAseConfigParameterPtr          pConfigParameters );


    /** Re-configure the location set monitor */
    void configure( IAseConfigParameterPtr pConfigParameters );


    /**
     * IThroughputPredictor interfaces
     */

    /**
     * obtain the predicted throughput from historical data
     * @param[in] pCurrentLocation The current LocationSetMonitor
     * @param[in] current The current short-term throughput
     * @param[out] confidence: a float number between 0 and 1 to indicate the confidence
     *                         level of the estimated throughput
     */
    virtual uint32_t predictThroughput( IThroughputPredictorPtr     pCurrentLocation,
                                        uint32_t                    current,
                                        uint32_t                    currentBufferLevel,
                                        double&                     confidence );

    /**
     * get the confidence value for the historical information
     */
    virtual double      getConfidence();

    /**
     * ILocationSetMonitor interfaces
     */
    virtual void        indicateBytesReceived(  MediaType mediaType,
                                                uint32_t bytes,
                                                bool drainingSocketReceiveBuffer,
                                                AseTimeVal earliestTime );

    virtual void        indicateReceptionStopped( MediaType mediaType );

    virtual void        addConnectInfo( AseTimeVal const&   connectTime,
                                        std::string const&  ipAddr );
    virtual void        addConnectionFailure();

    virtual uint32_t    getShortTermThroughput( AseTimeVal const& now, uint32_t currentBufferLevel );

    virtual void        connectionStat( uint32_t& avgConnectTime, uint32_t& varConnectTime,
                                        uint32_t& numberOfRecentConnectionFailures);

    virtual void        setNetworkInterfaceInfo( NetworkInterfaceInfo const& netinfo );

    virtual void        setClientExternalIpAddress( std::string const& ipaddr );

    virtual std::string getLocationSetKey();

    Time          getLastObservedTime() const;
    virtual void purge();
    virtual void probeDone();
    virtual bool eligibleForProbing() const;

    virtual Variant getState();

    virtual Variant getStateParams() const;

    virtual void        setState( Variant const& state );

    virtual void        setListener( ILocationSetMonitor::ListenerPtr pListener );

    virtual std::string const& name() const;

    virtual uint32_t    getLevel() const;

    virtual void        setLevel( uint32_t level );

    virtual std::string getIpAddr();

    /**
     * returns the distribution
     */
    shared_ptr<IThroughputDistribution> getDistribution( AseTimeVal const& now ) const;

protected:

    LocationSetMonitor( std::string const&              locationSetKey,
                        shared_ptr<ThroughputHistory> pThroughputHistory,
                        Params const& params);

protected:

    typedef std::vector<std::pair<uint32_t, AverageThroughput> > ThroughputsList;

    std::string const   mLocationSetKey;

    Params              mParams;

    uint32_t            mLevel;

    std::string         mIpAddr;

    bool                mAudioPaused;

    bool                mVideoPaused;

    bool                mVideoResuming;

    ThroughputsList     mThroughputs;

    AseTimeVal     mFirstReceptionTime;

    AseTimeVal     mAdjustedFirstReceptionTime;

    AseTimeVal     mLastReceptionTime;

    AseTimeVal     mReceptionStoppedTime;

    AseTimeVal     mLastHistoryUpdateTime;

    uint32_t            mBytesReceived;

    ILocationSetMonitor::ListenerPtr mListenerPtr;

    shared_ptr<ThroughputHistory>   mHistory;

    AseTimeVal     mThroughputCalculationTime;

    uint32_t            mThroughputCalculation;

    AseTimeVal     mHistoryPredictionTime;

    uint32_t            mHistoryPrediction;

    double              mHistoryConfidence;

    std::deque<AseTimeVal> mConnectionFailureTimePoints;

    AseTimeVal     mPrevProbeCompleteTime;
};

typedef LocationSetMonitor::LocationSetMonitorPtr LocationSetMonitorPtr;

}}

#endif
