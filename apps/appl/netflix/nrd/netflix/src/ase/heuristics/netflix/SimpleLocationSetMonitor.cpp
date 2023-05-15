/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SimpleLocationSetMonitor.h"
#include <nrdase/AseCommonUtil.h>
#include <nrdbase/Time.h>
#include <nrdbase/tr1.h>

#include "NetworkMonitor.h"

#include <limits>
#include <algorithm>

#define DEBUG_LOCATIONSETMONITOR


using namespace netflix::ase;
using namespace std;
using namespace netflix;

namespace netflix {
namespace ase {
DEFINE_LOGGER( SimpleLocationSetMonitorLog );
}
}

static llong variantAsInt( Variant const& v )
{
    return v.isInteger() ? v.integer() : ((llong) v.dbl() );
}


SimpleLocationSetMonitor::LocationRecord::LocationRecord( std::string const& key ) :
        mBandwidth(0),
        mConfidence(0.0),
        mAvgConnectTime(0),
        mVarConnectTime(0),
        mRecentConnectFailureTime(AseTimeVal::ZERO),
        mTimestamp(),
        mLocationSetKey( key ),
        mIp("")
{
}

SimpleLocationSetMonitor::LocationRecord::LocationRecord( Variant const& record ) :
        mBandwidth( variantAsInt( record["b"] ) ),
        mConfidence( record.mapValue( "c", 0, (double) 1.0 ) ),
        mAvgConnectTime( variantAsInt( record["ct"] ) ),
        mVarConnectTime( variantAsInt( record["cv"] ) ),
        mRecentConnectFailureTime( AseTimeVal::convertFrom( Time::fromMS( variantAsInt( record["rft"] )))) ,
        mTimestamp( AseTimeVal::convertFrom( Time::fromMS( variantAsInt( record["t"] ) ) ) ),
        mLocationSetKey( record["key"].string() ),
        mIp( record["ip"].string() )
{
}

SimpleLocationSetMonitor::LocationRecord&
SimpleLocationSetMonitor::LocationRecord::operator=( SimpleLocationSetMonitor::LocationRecord const& rhs )
{
    assert( mLocationSetKey == rhs.mLocationSetKey );

    mBandwidth = rhs.mBandwidth;
    mConfidence = rhs.mConfidence;
    mAvgConnectTime = rhs.mAvgConnectTime;
    mVarConnectTime = rhs.mVarConnectTime;
    mRecentConnectFailureTime = rhs.mRecentConnectFailureTime;
    mTimestamp = rhs.mTimestamp;
    mIp = rhs.mIp;

    return *this;
}

Variant SimpleLocationSetMonitor::LocationRecord::toVariant() const
{
    Variant record;
    record["b"] = mBandwidth;
    record["c"] = mConfidence;
    record["ct"] = mAvgConnectTime;
    record["cv"] = mVarConnectTime;
    // recent failure time
    record["rft"] = (llong) (mRecentConnectFailureTime.convertTo<Time>().ms());
    record["t"] = (llong) ( mTimestamp.convertTo<Time>().ms() );
    record["key"] = mLocationSetKey;
    record["ip"] = mIp;

    return record;
}

SimpleLocationSetMonitor::ThroughputRecord::ThroughputRecord() :
                mTotalBytes(0),
                mTotalTime(AseTimeVal::ZERO)
{
}

SimpleLocationSetMonitorPtr SimpleLocationSetMonitor::create(
                                std::string const&              locationSetKey,
                                IAseConfigParameterPtr const&   pConfigParameter )
{
    DEBUGLOG(SimpleLocationSetMonitorLog,"Create new SimpleLocationSetMonitor for %s",
        locationSetKey.c_str() );

    return  SimpleLocationSetMonitorPtr( new SimpleLocationSetMonitor( locationSetKey, pConfigParameter ) );
}

SimpleLocationSetMonitor::SimpleLocationSetMonitor(
        std::string const& locationSetKey,
        IAseConfigParameterPtr const& pConfigParameter )
    : mHistoricBandwidthUpdateInterval( AseTimeVal::fromMS( 10000 ) ),
      mLocationSetKey(locationSetKey),
      mConfigParameterPtr(pConfigParameter),
      mTotalBytes(0),
      mTotalTime(AseTimeVal::ZERO),
      mCurrentThroughput(0),
      mCurrentThroughputUpdateTime(AseTimeVal::now()),
      mLastUpdateTime( AseTimeVal::now() ),
      mLastHistoricBandwidthUpdate( AseTimeVal::ZERO ),
      mIpAddr(""),
      mLevel( NetworkMonitor::UNKNOWN_LEVEL ),
      mReceptionStopped(true),
      mNewlyReceivedBytes(0),
      mHasValidEstimate(false),
    mAverageConnectTime( 0 ),
    mTotalConnectTime( 0 ),
    mNumOfConnectTimeSamples(0),
    mAverageSquareConnectTime( 0 ),
    mLocationSetRecord( locationSetKey ),
    mPrevProbeCompleteTime(AseTimeVal::ZERO),
    mLatestSampleQueueUpdateTime(AseTimeVal::ZERO)
{
    configure( mConfigParameterPtr );
}

void SimpleLocationSetMonitor::configure( IAseConfigParameterPtr pConfigParameters )
{
    mConfigParameterPtr = pConfigParameters;

    mThroughputMeasureSize = mConfigParameterPtr->getParameterAs<uint32_t>(
                                "initThroughputMeasureDataSize", 128 * 1024);

    mThroughputMeasurementTimeout = mConfigParameterPtr->getParameterAs<uint32_t>(
                                "throughputMeasurementTimeout", 2000);
    mMaxTcpRecvBuffer = mConfigParameterPtr->getParameterAs<uint32_t>(
                                "socketReceiveBufferSize", 128 * 1024);
    mFastHistoricBandwidthExpirationTime = mConfigParameterPtr->getParameterAs<uint32_t>(
                                            "fastHistoricBandwidthExpirationTime",
                                            0,
                                            24 * 3600,
                                            600 );
    mBandwidthExpirationTime = mConfigParameterPtr->getParameterAs<uint32_t>(
                                    "bandwidthExpirationTime", 0, 0xFFFFFFFF, 7 * 24 * 3600 );

    mHistoricBandwidthUpdateInterval = AseTimeVal::fromMS( mConfigParameterPtr->getParameterAs<uint32_t>(
                "historicBandwidthUpdateInterval", 1000, MAX_INT32, 10000 ) );

    mGapToInsertSample = AseTimeVal::fromMS(100);

    mLocationEstimateMargin = mConfigParameterPtr->getParameterAs<uint32_t>(
                                    "locationEstimateMargin", 25);
    mThroughputLowestConfidenceLevel = mConfigParameterPtr->getParameterAs<double>(
                                    "throughputLowestConfidenceLevel", 0.2);
    mThroughputLowConfidenceLevel =
        mConfigParameterPtr->getParameterAs<double>("throughputLowConfidenceLevel", 0.4);

    int const intmax = std::numeric_limits<int32_t>::max();
    mConnectTimeHalflife = mConfigParameterPtr->getParameterAs<uint32_t>( "connectTimeHalflife", 1, intmax, 20 );
    mConnectionFailureExpirationTime =
        mConfigParameterPtr->getParameterAs<uint32_t>("connectionFailureExpirationTime", 1, intmax, 60 * 1000); // 1 minute

    mConnectTimeAlpha = 1.0 - pow( 0.5, 1.0 / mConnectTimeHalflife );

    mLockPeriodAfterProbeComplete =
        mConfigParameterPtr->getParameterAs<uint32_t>("lockPeriodAfterProbeComplete", 0, intmax, DEFAULT_LOCK_PERIOD_AFTER_PROBE_COMPLETE);

    mThroughputMesurementWindowMap = mConfigParameterPtr->loadConfigMap("throughputMeasurementWindowCurve", "b", "w", 1000);

    uint32_t configedMeasureWindow = mConfigParameterPtr->getParameterAs<uint32_t>(
                                "throughputMeasureWindow", 1000, MAX_INT32, DEFAULT_THROUGHPUT_MEASURE_WINDOW);

    std::map<uint32_t, shared_ptr<ThroughputRecord> > throughputRecordMap;

    for ( map<uint32_t, uint32_t>::iterator iter = mThroughputMesurementWindowMap.begin();
            iter != mThroughputMesurementWindowMap.end();
            ++iter )
    {
        uint32_t const measureWindow = iter->second;

        std::map<uint32_t, shared_ptr<ThroughputRecord> >::iterator it = mThroughputRecordMap.find( measureWindow );
        if ( it != mThroughputRecordMap.end() )
        {
            DEBUGLOG(SimpleLocationSetMonitorLog,"retain existing records for %s %ums",
                    mLocationSetKey.c_str(),
                    measureWindow );

            throughputRecordMap[ measureWindow ] = it->second;
        }
        else
        {
            DEBUGLOG(SimpleLocationSetMonitorLog,"new records for %s %ums",
                    mLocationSetKey.c_str(),
                    measureWindow );

            throughputRecordMap[ measureWindow ].reset( new ThroughputRecord() );
        }

        if ( measureWindow > configedMeasureWindow )
            configedMeasureWindow = measureWindow;
    }

    mThroughputRecordMap = throughputRecordMap;

    mMaximumThroughputMeasureWindow = AseTimeVal::fromMS(configedMeasureWindow);

    if ( mThroughputRecordMap.empty() )
    {
        mThroughputRecordMap[configedMeasureWindow].reset(new ThroughputRecord());
    }

}

void SimpleLocationSetMonitor::indicateBytesReceived(   MediaType mediaType,
                                                        uint32_t bytes,
                                                        bool drainingSocketReceiveBuffer,
                                                        AseTimeVal earliestTime )
{
    NRDP_UNUSED(earliestTime);
    if ( drainingSocketReceiveBuffer )
    {
        // ignore the bytes in the socket receive buffer
        if ( !mReceptionStopped )
            indicateReceptionStopped(mediaType);

        return;
    }
    AseTimeVal now = AseTimeVal::now();
    mMeasuredTime = now;

#ifdef DEBUG_LOCATIONSETMONITOR_VERBOSE
    DEBUGLOG(SimpleLocationSetMonitorLog, "indicateBytesReceived: media type: %u, bytes: %u, draining: %u",
                                mediaType, bytes, drainingSocketReceiveBuffer);
#endif

    do
    {
        if ( mReceptionStopped )
        {
            // ignore the first sample after resuming the reception to set lastSampleTime correctly
            //
            // ignore all audio samples before the first video sample after resuming the reception
            // We do not measure the throughput when the player is only downloading the audio sample
            if ( mediaType == MEDIA_VIDEO )
            {
                mReceptionStopped = false;
                mCurrentSample.start = now;
                mCurrentSample.bytes = 0;
            }
            break;
        }


        // Add a sample
        mCurrentSample.end = now;
        mCurrentSample.bytes += bytes;
        mNewlyReceivedBytes += bytes;
        if (now > mCurrentSample.start + AseTimeVal::fromMS(AGGREGATE_SAMPLE_SIZE))
        {
            // aggregate mini-samples into big samples
#ifdef DEBUG_LOCATIONSETMONITOR_VERBOSE
            DEBUGLOG(SimpleLocationSetMonitorLog, "pushOneSample, %llu ms, %u bytes, 1",
                    (mCurrentSample.end - mCurrentSample.start).ms(), mCurrentSample.bytes);
#endif
            pushOneSample(mSamples,
                          mCurrentSample);
            mCurrentSample.start = now;
            mCurrentSample.bytes = 0;

            resizeSampleQueue(mSamples, mMaximumThroughputMeasureWindow);
        }


    } while (false);

    // report our state if anyone is listening
    if ( mListenerPtr && ( now - mLastUpdateTime ).ms() > 2000 )
    {
        // XXX getState calls getPredictedThroughput, which can call
        // indicateBytesReceived to fudge the numbers, so we can end up back here.
        // Therefore it's important to update the time first
        mLastUpdateTime = now;
        mListenerPtr->monitorUpdate( mLocationSetKey, getState(), getStateParams() );
    }

    // update the historyic bandwidth
    if ( now - mLastHistoricBandwidthUpdate > mHistoricBandwidthUpdateInterval )
    {
        if ( mLastHistoricBandwidthUpdate == AseTimeVal::ZERO )
        {
            mLastHistoricBandwidthUpdate = now;
        }
        else
        {
            mLastHistoricBandwidthUpdate = now;

            updateHistory();
        }
    }

    // update the last sample time
    mLastSampleTime = now;
}

void
SimpleLocationSetMonitor::pushOneSample(
        std::deque<Sample>& sampleQueue,
        Sample const& sample)
{
    sampleQueue.push_back(sample);
    mTotalBytes += sample.bytes;
    AseTimeVal timeElapsed = sample.end - sample.start;
    mTotalTime += timeElapsed;

    shared_ptr<ThroughputRecord> pThroughputRecord;
    for ( map<uint32_t, shared_ptr<ThroughputRecord> >::iterator iter = mThroughputRecordMap.begin();
            iter != mThroughputRecordMap.end();
            ++iter )
    {
        pThroughputRecord = iter->second;
        pThroughputRecord->mTotalBytes += sample.bytes;
        pThroughputRecord->mTotalTime += timeElapsed;
    }

    return;
}


void
SimpleLocationSetMonitor::resizeSampleQueue(std::deque<Sample>& sampleQueue,
                                       AseTimeVal const& measureWindow)
{
    AseTimeVal now = AseTimeVal::now();

    if ( mLatestSampleQueueUpdateTime + AseTimeVal::fromMS(SAMPLE_QUEUE_UPDATE_INTERVAL) < now )
    {
        // Resize to max samples and/or max time.
        Sample s;
        while ( (sampleQueue.size() > MAX_SAMPLES_NUM)
                || (mTotalTime > measureWindow) )
        {
            s = sampleQueue.front();
            mTotalBytes -= s.bytes;
            mTotalTime -= (s.end - s.start);
            sampleQueue.pop_front();
        }

        shared_ptr<ThroughputRecord> pThroughputRecord;
        AseTimeVal measureWindow;
        for ( map<uint32_t, shared_ptr<ThroughputRecord> >::iterator iter = mThroughputRecordMap.begin();
                iter != mThroughputRecordMap.end();
                ++iter )
        {
            measureWindow = AseTimeVal::fromMS(iter->first);
            pThroughputRecord = iter->second;
            AseTimeVal timeToElapse, timeElapsed;
            ullong bytesElapsed = 0;
            if ( mTotalTime > measureWindow )
            {
                timeToElapse = mTotalTime - measureWindow;
                timeElapsed = AseTimeVal::ZERO;
                bytesElapsed = 0;
                for ( deque<Sample>::iterator sampleIter = sampleQueue.begin();
                        sampleIter != sampleQueue.end();
                        ++sampleIter )
                {
                    AseTimeVal sampleTime = (sampleIter->end - sampleIter->start);
                    if ( timeElapsed + sampleTime > timeToElapse )
                    {
                        break;
                    }
                    else
                    {
                        timeElapsed += sampleTime;
                        bytesElapsed += sampleIter->bytes;
                    }
                }
                assert(mTotalBytes >= bytesElapsed);
                assert(mTotalTime >= timeElapsed);
                pThroughputRecord->mTotalBytes = mTotalBytes - bytesElapsed;
                pThroughputRecord->mTotalTime = mTotalTime - timeElapsed;
            }
            else
            {
                pThroughputRecord->mTotalBytes = mTotalBytes;
                pThroughputRecord->mTotalTime = mTotalTime;
            }
        }
        mLatestSampleQueueUpdateTime = now;
    }
    return;

}

void SimpleLocationSetMonitor::indicateReceptionStopped(MediaType mediaType)
{
    if ( mediaType == MEDIA_AUDIO )
        return;

    if ( !mReceptionStopped )
    {
        mCurrentSample.end = AseTimeVal::now();
#ifdef DEBUG_LOCATIONSETMONITOR_VERBOSE
        DEBUGLOG(SimpleLocationSetMonitorLog, "pushOneSample, %llu ms, %u bytes, 2",
                (mCurrentSample.end - mCurrentSample.start).ms(), mCurrentSample.bytes);
#endif
        pushOneSample(mSamples, mCurrentSample);
    }

#ifdef DEBUG_LOCATIONSETMONITOR
    DEBUGLOG(SimpleLocationSetMonitorLog, "indicateReceptionStopped: %u", mediaType);
#endif
    mReceptionStopped = true;
    mNewlyReceivedBytes = 0;
    mCurrentSample.start = AseTimeVal::now();
    mCurrentSample.bytes = 0;
}

void SimpleLocationSetMonitor::addConnectInfo( AseTimeVal const& connectTime, std::string const& ipAddr )
{
    // update connect time statistics
    ullong connectTimeUs = connectTime.us();
    mAverageConnectTime = static_cast<ullong> (mAverageConnectTime * ( 1.0 - mConnectTimeAlpha ) + mConnectTimeAlpha * connectTimeUs);
    mTotalConnectTime += connectTimeUs;
    ++mNumOfConnectTimeSamples;
    mAverageSquareConnectTime = static_cast<ullong> ( mAverageSquareConnectTime * ( 1.0 - mConnectTimeAlpha ) + mConnectTimeAlpha * connectTimeUs * connectTimeUs);
#ifdef DEBUG_LOCATIONSETMONITOR_VERBOSE
    DEBUGLOG(SimpleLocationSetMonitorLog,
            "addConnectInfo: connect time: %llu ms, update average connect time: %llu ms, average square connect time: %llu ms",
            connectTimeUs / 1000, mAverageConnectTime / 1000, mAverageSquareConnectTime / 1000);
#endif

    mIpAddr = ipAddr;
}

void SimpleLocationSetMonitor::addConnectionFailure()
{
    mConnectionFailureTimePoints.push_back(AseTimeVal::now());
}

uint32_t
SimpleLocationSetMonitor::lookupMeasuredWindow(uint32_t bufferLevel)
{
    if ( mThroughputMesurementWindowMap.empty() )
        return mMaximumThroughputMeasureWindow.ms();

    map<uint32_t, uint32_t>::iterator windowIter = mThroughputMesurementWindowMap.upper_bound(bufferLevel);

    uint32_t measureWindow;
    if ( windowIter != mThroughputMesurementWindowMap.end() )
        measureWindow = windowIter->second;
    else
        measureWindow = mThroughputMesurementWindowMap.rbegin()->second;

    return measureWindow;
}


uint32_t SimpleLocationSetMonitor::getShortTermThroughput( AseTimeVal const& now, uint32_t currentBufferLevel )
{
    if ( (now < mCurrentThroughputUpdateTime + AseTimeVal::fromMS(THROUGHPUT_UPDATE_INTERVAL)) &&
            (mCurrentThroughput > 0) )
    {
        return mCurrentThroughput;
    }

    if ( !mReceptionStopped && (now > mCurrentSample.start + AseTimeVal::fromMS(AGGREGATE_SAMPLE_SIZE)) )
    {
        mCurrentSample.end = now;
#ifdef DEBUG_LOCATIONSETMONITOR_VERBOSE
        DEBUGLOG(SimpleLocationSetMonitorLog, "pushOneSample, %llu ms, %u bytes, 3",
                (mCurrentSample.end - mCurrentSample.start).ms(), mCurrentSample.bytes);
#endif
        pushOneSample(mSamples, mCurrentSample);
        mCurrentSample.start = now;
        mCurrentSample.bytes = 0;
        resizeSampleQueue(mSamples, mMaximumThroughputMeasureWindow);
    }

    uint32_t throughput = 0;

    uint32_t measuredWindow = lookupMeasuredWindow(currentBufferLevel);
    shared_ptr<ThroughputRecord> pThroughputRecord = mThroughputRecordMap[measuredWindow];
    ullong totalBytes = pThroughputRecord->mTotalBytes;
    AseTimeVal totalTime = pThroughputRecord->mTotalTime;

    uint32_t minMeasureTime = min(1000, (int32_t)measuredWindow / 2);

    if ( (totalTime.ms() >= minMeasureTime) || (totalBytes > MIN_SAMPLES_TOTAL_SIZE) )
    {
        if (totalTime.ms() > 0 )
        {
            throughput = totalBytes * 8 / totalTime.ms();
        }
        else if ( totalBytes > 0 )
        {
            // It takes the player less than 1 ms to download the data. Just have a reasonable guess.
            throughput = 5 * 1024;
        }

        //Not clear why measured time should be updated just because we read the throughput ??
        //mMeasuredTime = AseTimeVal::now();
    }

#ifdef DEBUG_LOCATIONSETMONITOR
    DEBUGLOG(SimpleLocationSetMonitorLog,
            "getShortTermThroughput:"
            " %s, total bytes: %llu, throughput: %ukbps, queue size: %zu "
            "total time: %llums, measured window: %u ms, buffer level: %u ms",
            mLocationSetKey.c_str(), totalBytes, throughput, mSamples.size(),
            totalTime.ms(), measuredWindow, currentBufferLevel );
#endif

    for ( map<uint32_t, shared_ptr<ThroughputRecord> >::iterator iter = mThroughputRecordMap.begin();
            iter != mThroughputRecordMap.end();
            ++iter )
    {
        pThroughputRecord = iter->second;
#ifdef DEBUG_LOCATIONSETMONITOR
        if ( pThroughputRecord->mTotalTime.ms() > 0 )
        {
            DEBUGLOG(SimpleLocationSetMonitorLog,
                    "throughput window: %u, throughput: %llu kbps",
                    iter->first, pThroughputRecord->mTotalBytes * 8 / pThroughputRecord->mTotalTime.ms() );
        }
#endif
    }

    mCurrentThroughput = throughput;
    mCurrentThroughputUpdateTime = now;
    return throughput;
}

uint32_t SimpleLocationSetMonitor::predictThroughput(   IThroughputPredictorPtr     pCurrentLocationSet,
                                                        uint32_t                    currentThroughput,
                                                        uint32_t                    currentBufferLevel,
                                                        double&                     confidence )
{
    NRDP_UNUSED( pCurrentLocationSet );
    NRDP_UNUSED( currentThroughput );

    AseTimeVal const now = AseTimeVal::now();
    int throughput = getShortTermThroughput( now, currentBufferLevel );

    confidence = 0.0;
    if ( throughput > 0 )
    {
        if (mNewlyReceivedBytes >= mThroughputMeasureSize)
        {
            confidence = 1.0;
            mHasValidEstimate = true;
        }
        else
        {
            confidence = 0.7;
        }
    }
    else if ( mLocationSetRecord.mBandwidth > 0 )
    {
        AseTimeVal now = AseTimeVal::now();
        // DEBUGLOG( SimpleLocationSetMonitorLog, "obtainThroughput: historic throughput");
        throughput = mLocationSetRecord.mBandwidth;
        //Not clear why measured time should be updated just because we read the throughput ??
        //mMeasuredTime = mLocationSetRecord.mTimestamp;
        //

        if ( mLocationSetRecord.mTimestamp + AseTimeVal::fromSeconds(mFastHistoricBandwidthExpirationTime) > now )
        {
            confidence = std::min( mLocationSetRecord.mConfidence, 0.75 );
        }
        else if ( mLocationSetRecord.mTimestamp + AseTimeVal::fromSeconds(mBandwidthExpirationTime) > now )
        {
            confidence = std::min( mLocationSetRecord.mConfidence, 0.3 );
        }
        else
        {
            confidence = std::min( mLocationSetRecord.mConfidence, 0.1 );
        }

        AseTimeVal mostRecentConnectFailureTime = getMostRecentConnectFailureTime(mLocationSetRecord.mRecentConnectFailureTime);

        if ( mostRecentConnectFailureTime + AseTimeVal::fromMS(mConnectionFailureExpirationTime) > now )
        {
            // have a recent network failure
            confidence = 0.0;
        }
    }


    return throughput;
}

double SimpleLocationSetMonitor::getConfidence()
{
    double confidence;
    predictThroughput(IThroughputPredictorPtr(), 0, 0, confidence);
    return confidence;
}

void SimpleLocationSetMonitor::connectionStat( uint32_t& avgConnectTime, uint32_t& varConnectTime,
                                               uint32_t& numberOfRecentConnectionFailures)
{
    if ( mAverageConnectTime > 0 )
    {
        AseTimeVal now = AseTimeVal::now();
        AseTimeVal failureExpirationTime = AseTimeVal::fromMS(mConnectionFailureExpirationTime);
        if ( (mNumOfConnectTimeSamples > mConnectTimeHalflife) || !mNumOfConnectTimeSamples )
        {
            avgConnectTime = mAverageConnectTime / 1000;
        }
        else
        {
            avgConnectTime = mTotalConnectTime / mNumOfConnectTimeSamples / 1000;
        }
        varConnectTime = ( mAverageSquareConnectTime - mAverageConnectTime * mAverageConnectTime ) / 1000;
        while ( !mConnectionFailureTimePoints.empty() )
        {
            AseTimeVal failureTime = mConnectionFailureTimePoints.front();
            if ( failureTime + failureExpirationTime < now )
            {
                // the failure is too old
                mConnectionFailureTimePoints.pop_front();
            }
            else
            {
                break;
            }
        }
        numberOfRecentConnectionFailures = mConnectionFailureTimePoints.size();
    }
    else
    {
         avgConnectTime = mLocationSetRecord.mAvgConnectTime;
         varConnectTime = mLocationSetRecord.mVarConnectTime;
         numberOfRecentConnectionFailures = 0;
    }
}

void SimpleLocationSetMonitor::purge()
{
    mSamples.clear();
    mTotalBytes = 0;
    mTotalTime = AseTimeVal::ZERO;
    mCurrentSample.start = AseTimeVal::now();
    mCurrentSample.bytes = 0;

    mPrevProbeCompleteTime = AseTimeVal::ZERO;
}

void SimpleLocationSetMonitor::probeDone()
{
    mPrevProbeCompleteTime = AseTimeVal::now();

}

bool SimpleLocationSetMonitor::eligibleForProbing() const
{
    if ( mPrevProbeCompleteTime + AseTimeVal::fromMS(mLockPeriodAfterProbeComplete) > AseTimeVal::now() )
    {
        // in the lock period
        return false;
    }
    else
    {
        return true;
    }
}

void SimpleLocationSetMonitor::updateHistory()
{
    uint32_t avgConnectTime, varConnectTime, numberOfConnectionFailures;
    connectionStat(avgConnectTime, varConnectTime,numberOfConnectionFailures);
    double confidence;
    uint32_t throughput = predictThroughput(    IThroughputPredictorPtr(),
                                                0,
                                                mMaximumThroughputMeasureWindow.ms(),
                                                confidence );
    if ( throughput > 0 )
    {
        mLocationSetRecord.mBandwidth = throughput;
        mLocationSetRecord.mConfidence = confidence;
        mLocationSetRecord.mAvgConnectTime = avgConnectTime;
        mLocationSetRecord.mVarConnectTime = varConnectTime;
        mLocationSetRecord.mTimestamp = mMeasuredTime;
        mLocationSetRecord.mRecentConnectFailureTime = getMostRecentConnectFailureTime(mLocationSetRecord.mRecentConnectFailureTime);
        mLocationSetRecord.mIp = mIpAddr;
    }
}


Variant SimpleLocationSetMonitor::getState()
{
    updateHistory();

    Variant const state = mLocationSetRecord.toVariant();

    DEBUGLOG(SimpleLocationSetMonitorLog, "SimpleLocationSetMonitor::getState() %s: %s",
                    mLocationSetKey.c_str(), state.toJSON().c_str() );

    return state;
}

Variant SimpleLocationSetMonitor::getStateParams() const
{
    return Variant();
}

void SimpleLocationSetMonitor::setState( Variant const& state )
{
    if ( state.contains( "key" ) && state["key"] == mLocationSetKey )
    {
        DEBUGLOG(SimpleLocationSetMonitorLog, "SimpleLocationSetMonitorLog::setState() %s : %s",
                    mLocationSetKey.c_str(), state.toJSON().c_str() );
        mLocationSetRecord = LocationRecord( state );
        mMeasuredTime = mLocationSetRecord.mTimestamp;
        mAverageConnectTime = mLocationSetRecord.mAvgConnectTime;
        mAverageSquareConnectTime = mLocationSetRecord.mVarConnectTime;
    }
    else
    {
        DEBUGLOG(SimpleLocationSetMonitorLog, "SimpleLocationSetMonitorLog::setState() %s : %s - no key or key mismatch",
                    mLocationSetKey.c_str(), state.toJSON().c_str() );
    }
}

string SimpleLocationSetMonitor::getLocationSetKey()
{
    return mLocationSetKey;
}

std::string const& SimpleLocationSetMonitor::name() const
{
    return mLocationSetKey;
}

string SimpleLocationSetMonitor::getIpAddr()
{
    return mIpAddr;
}

Time SimpleLocationSetMonitor::getLastObservedTime() const
{
    return mMeasuredTime.convertTo<Time>();
}

void SimpleLocationSetMonitor::setListener( ILocationSetMonitor::ListenerPtr pListener )
{
    mListenerPtr = pListener;
}

uint32_t SimpleLocationSetMonitor::getLevel() const
{
    return mLevel;
}

void SimpleLocationSetMonitor::setLevel( uint32_t level )
{
    mLevel = level;
}

void SimpleLocationSetMonitor::obtainTotalBytesAndTime(ullong& totalBytes, AseTimeVal& totalTime) const
{
    AseTimeVal const now = AseTimeVal::now();
    //Take the current sample, and any stall time, into the measurement
    if ( !mReceptionStopped )
    {
        totalBytes = mCurrentSample.bytes;
        totalTime = now - mCurrentSample.start;
    }
    else if (mCurrentSample.bytes > 0)
    {
        totalBytes = mCurrentSample.bytes;
        totalTime = (mCurrentSample.end - mCurrentSample.start);
    }

    // FIXME: only support short term throughput only
    totalBytes += mTotalBytes;
    totalTime += mTotalTime;
    return;
}

AseTimeVal
SimpleLocationSetMonitor::getMostRecentConnectFailureTime(AseTimeVal const& storedConnectFailureTime)
{
    if ( mConnectionFailureTimePoints.empty() )
        return storedConnectFailureTime;
    else
    {
        return mConnectionFailureTimePoints.back();
    }

}
