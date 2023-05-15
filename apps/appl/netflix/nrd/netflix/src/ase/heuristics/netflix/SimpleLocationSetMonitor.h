/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SIMPLE_LOCATION_SET_MONITOR_H
#define SIMPLE_LOCATION_SET_MONITOR_H

#include <nrdase/ILocationSetMonitor.h>

#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/INetworkMonitor.h>
#include <nrdbase/Variant.h>
#include <nrdase/DebugLogging.h>

#include <string>
#include <deque>
#include <sstream>

namespace netflix {
namespace ase {
DECLARE_LOGGER( SimpleLocationSetMonitorLog );

class SimpleLocationSetMonitorAggregate;

/**
 * @class SimpleLocationSetMonitor
 * @brief A simple location set monitor, which uses run-time average to estimate the location's throughput
 *
 * state object = { "#mandatory" : [ "b", "ct", "cv", "t", "key", "ip" ],
                    "b" : "integer(0,-)",       // bandwidth kbit/s
                    "ct" : "integer(0,-)",      // connect time (ms)
                    "cv" : "integer(0,-)",      // connect time variance (ms^2)
                    "t" : "integer(0,-)",       // last observed time (ms since epoche)
                    "key" : "string",           // location set key
                    "ip" : "string"             // server ip address
                }
 */
class SimpleLocationSetMonitor : public ILocationSetMonitor
{
public:
    typedef SimpleLocationSetMonitorAggregate AggregateType;

    typedef shared_ptr<SimpleLocationSetMonitor> SimpleLocationSetMonitorPtr;

    static SimpleLocationSetMonitorPtr create(  std::string const&              locationSetKey,
                                                IAseConfigParameterPtr const&   pConfigParameter );

    /** Re-configure the location set monitor */
    void configure( IAseConfigParameterPtr pConfigParameters );

    /**
     * IThroughoutPredictor interface
     */

    /**
     * obtain the predicted throughput from historical data
     * @param[in] current The current short-term throughput
     * @param[in] current buffer level in ms
     * @param[out] confidence: a float number between 0 and 1 to indicate the confidence
     *                         level of the estimated throughput
     */
    virtual uint32_t predictThroughput( IThroughputPredictorPtr     pCurrentLocationSet,
                                        uint32_t                    currentThroughput,
                                        uint32_t                    currentBufferLevel,
                                        double&                     confidence ) ;

    /**
     * get the confidence value for the historical information
     */
    virtual double getConfidence();

    /**
     * ILocationSetMonitor interfaces
     */
    virtual void indicateBytesReceived( MediaType mediaType,
                                        uint32_t bytes,
                                        bool drainingSocketReceiveBuffer,
                                        AseTimeVal earliestTime );

    virtual void indicateReceptionStopped( MediaType mediaType );

    virtual void addConnectInfo( AseTimeVal const& connectTime,
                                 std::string const& ipAddr );
    virtual void addConnectionFailure();



    /** obtain the short-term throughput
     * @param[in] now The current time
     * @return The short-term throughput in kbit/s, or zero if insufficient recent data
     */
    virtual uint32_t getShortTermThroughput( AseTimeVal const& now, uint32_t currentBufferLevel );

    virtual void connectionStat(uint32_t& avgConnectTime, uint32_t& varConnectTime,
                                uint32_t& numberOfRecentConnectionFailures);

    virtual void setNetworkInterfaceInfo( NetworkInterfaceInfo const& /*netinfo*/ ) {}

    virtual void setClientExternalIpAddress( std::string const& /*ipaddr*/ ) {}

    virtual std::string getLocationSetKey();
    virtual std::string getIpAddr();
    virtual std::string const& name() const;

    virtual uint32_t getLevel() const;
    virtual void purge();
    virtual void probeDone();
    virtual bool eligibleForProbing() const;


    virtual Variant getState();

    virtual Variant getStateParams() const;

    virtual Time getLastObservedTime() const;

    virtual void setState( Variant const& state );

    virtual void setListener( ILocationSetMonitor::ListenerPtr pListener );

    void setLevel( uint32_t level );

    void setAggregateMonitor( shared_ptr<SimpleLocationSetMonitorAggregate> /* pAggregate */ ) {}

protected:

    SimpleLocationSetMonitor( std::string const& locationSetKey,
                              IAseConfigParameterPtr const& pConfigParameter );

    struct Sample
    {
        Sample(): bytes(0)
        {
            start = AseTimeVal::now();
            end = start;
        }

        uint32_t bytes; //!< Bytes read in this sample.
        AseTimeVal start; // start of the sample time
        AseTimeVal end; // end of the sample time

    };

    // Maximum number of measurement samples.
    static const uint32_t MAX_SAMPLES_NUM = 1024;
    static const uint32_t SAMPLE_QUEUE_UPDATE_INTERVAL = 500;
    static const uint32_t THROUGHPUT_UPDATE_INTERVAL = 500;
    // we can update the throughput estimate after the SimpleLocationSetMonitor received
    // at least MIN_SAMPLES_TOTAL_SIZE bytes
    static const uint32_t MIN_SAMPLES_TOTAL_SIZE = 96 * 1024;

    static const uint32_t AGGREGATE_SAMPLE_SIZE = 100;

    static const int32_t  MAX_INT32            = 0x7FFFFFFF;
    static const uint32_t MAX_UINT32           = 0xFFFFFFFF;

    struct LocationRecord
    {
        uint32_t    mBandwidth; // kbps
        double      mConfidence;
        uint32_t    mAvgConnectTime;
        uint32_t    mVarConnectTime;
        AseTimeVal  mRecentConnectFailureTime;
        AseTimeVal  mTimestamp;
        std::string const mLocationSetKey;
        std::string mIp;

        LocationRecord( std::string const& key );

        LocationRecord( Variant const& record );

        LocationRecord& operator=( LocationRecord const& rhs );

        Variant toVariant() const;
    };

    struct ThroughputRecord
    {
        ullong mTotalBytes;
        AseTimeVal mTotalTime;

        ThroughputRecord();
    };


    /**
     * push the sample into the given sampleQueue
     */
    void pushOneSample(std::deque<Sample>& sampleQueue,
                       Sample const& sample);
    /**
     * check and shrink the given sampleQueue to throughput measure window
     */
    void resizeSampleQueue(std::deque<Sample>& sampleQueue,
                           AseTimeVal const& measureWindow);

    void obtainTotalBytesAndTime(ullong& totalBytes, AseTimeVal& totalTime) const;

    uint32_t lookupMeasuredWindow(uint32_t bufferLevel);

    AseTimeVal getMostRecentConnectFailureTime(AseTimeVal const& storedFailureTime);

    void updateHistory();

protected:


    // the window size (in ms) to compute the current speed
    // 30 seconds is a guess at something large enough to skip over
    // temporary hiccups due to shared network use, but small enough to
    // lose old samples.
    AseTimeVal mMaximumThroughputMeasureWindow;
    uint32_t   mFastHistoricBandwidthExpirationTime;
    uint32_t   mBandwidthExpirationTime;
    uint32_t   mThroughputMeasureSize;
    AseTimeVal mHistoricBandwidthUpdateInterval;

    AseTimeVal mMeasuredTime;

    // We aggregate mini samples in to 1 second samples
    Sample mCurrentSample;

    std::string mLocationSetKey;

    IAseConfigParameterPtr mConfigParameterPtr;

    uint32_t mLocationEstimateMargin;
    double mThroughputLowestConfidenceLevel;
    double mThroughputLowConfidenceLevel;

    // samples_ only store throughputMeasureWindow_ ms worth of samples to compute speed
    std::deque<Sample> mSamples;


    ullong mTotalBytes;
    // total time elapsed in mSamples
    AseTimeVal mTotalTime;

    uint32_t mCurrentThroughput;
    AseTimeVal mCurrentThroughputUpdateTime;

    // last sample arrival time
    AseTimeVal mLastSampleTime;

    // last time a listener update was sent
    AseTimeVal mLastUpdateTime;

    // last time we updated the historic bandwidth
    AseTimeVal mLastHistoricBandwidthUpdate;

    // If the player does not receive any data within this period, a small sample will be inserted
    AseTimeVal mGapToInsertSample;

    std::string mIpAddr;

    uint32_t mLevel;

    bool mReceptionStopped;
    // maximum time (ms) to reach confidence 1.0
    uint32_t   mThroughputMeasurementTimeout;
    ullong mMaxTcpRecvBuffer;
    ullong mNewlyReceivedBytes;
    bool mHasValidEstimate;

    std::deque<AseTimeVal> mConnectionFailureTimePoints;


    uint32_t mConnectTimeHalflife;
    uint32_t mConnectionFailureExpirationTime;

    ullong            mAverageConnectTime;
    ullong            mTotalConnectTime;
    ullong            mNumOfConnectTimeSamples;

    ullong            mAverageSquareConnectTime;

    double mConnectTimeAlpha;

    LocationRecord mLocationSetRecord;

    ILocationSetMonitor::ListenerPtr mListenerPtr;

    // The player is not allowed to probe this location again during lockPeriodAfterProbeComplete
    // after a probe is complete
    AseTimeVal     mPrevProbeCompleteTime;
    uint32_t            mLockPeriodAfterProbeComplete;

    std::map<uint32_t, uint32_t> mThroughputMesurementWindowMap; // buffer length --> throughput window size


    AseTimeVal mLatestSampleQueueUpdateTime;

    std::map<uint32_t, shared_ptr<ThroughputRecord> > mThroughputRecordMap; // throughput window size --> throughput record
};

typedef SimpleLocationSetMonitor::SimpleLocationSetMonitorPtr SimpleLocationSetMonitorPtr;

}}

#endif
