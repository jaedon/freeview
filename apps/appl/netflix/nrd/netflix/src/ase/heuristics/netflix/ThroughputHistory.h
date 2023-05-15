/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef THROUGHPUT_HISTORY
#define THROUGHPUT_HISTORY

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/DebugLogging.h>

#include "ThroughputDistribution.h"

namespace netflix {

class Variant;
struct NetworkInterfaceInfo;


namespace ase {

DECLARE_LOGGER( ThroughputHistoryLog );

class IAseConfigParameter;
class ThroughputDistributionPredictor;

/**
 * @brief Class capturing throughput history for a LocationSet
 *
 * We keep separate history for each client network connection. Network connection is characterized by
 * - Interface name
 * - Mobile Country Code and Mobile Network Code if Wireless WAN
 * - SSID if Wireless LAN and SSID available
 * - Client external IP address, if known and if SSID/MCC/MNC not available
 *
 * Sometimes the client external IP address is not known. Specifically after a network change during
 * streaming we will not know the new client external IP address until the next NCCP transaction at the
 * earliest.
 *
 * If the client external IP address is now known then we use the most recent seen value.
 *
 * For each network connection, we keep history globally and by time of day.
 *
   params object = { "#mandatory" : [ "cf", "cm", "eca" ],
                     "cf" : "double",                       // confidence factor
                     "cm" : "double(0,1)",                  // minimum confidence
                     "eca" : "double(0,1)",                 // effective count alpha
                   };

   state array = [ { "#mandatory" : [ "k", "h" ],
                     "k" : "string",                        // network connection key
                     "h" : connectionhistory
                    } ]
    connectionhistory = {   "#mandatory" : [ "g", "tod" ],
                            "g" : history,                  // global history
                            "tod" : [ history ]             // time of day histories
                        }
    history = { "#mandatory" : [ "t", "cn", "cs", "d" ],
                "t" : "integer(0,-)",                       // last observed (ms since epoche)
                "cn" : "integer(0,-)",                      // average connection time (ms)
                "cs" : "integer(0,-)",                      // average of connect time squared (ms)
                "d" : distribution
              }

    distribution = {    "#mandatory" : [ "c", "a", "b", "as", "cl", "m", "ec", "ect" ],
                        "c" : "integer",                    // measurement count
                        "a" : "integer",                    // average throughput
                        "as" : "integer",                   // average of (throughput squared)
                        "b" : "double",                     // cluster bandwidth
                        "cl" : [ cluster ],                 // clusters
                        "m" : "integer",                    // weight multiplier
                        "ec" : "double",                    // effective count
                        "ect" : "integer(0,-)"              // effective count observation time (ms since epoch)
                    }

    cluster = [ "integer", "integer", "integer" ]           // mean, count, weight * multiplier


 */
class ThroughputHistory
{
public:

    struct Params
    {
        int       mTimeOfDayGranularity;    // The number of hours of the day for each history object
                                            // must be a divisor of 24
        double    mConnectTimeAlpha;
        ThroughputDistribution::Params mThroughputDistributionParams;

        AseTimeVal mConnectionFailureExpirationTime;

        Params();

        void initialize( shared_ptr<IAseConfigParameter> pConfigParameters );
    };

    typedef shared_ptr<ThroughputHistory> ThroughputHistoryPtr;

    ThroughputHistory( Params const& params,
                       shared_ptr<ThroughputDistributionPredictor> pPredictor );

    /** Re-configure */
    void configure( IAseConfigParameterPtr pConfigParameters );

    /**
     * Obtain the predicted throughput from historical data
     * @param[in] Current time
     * @param[in] current The current short-term throughput, zero if not known
     * @param[out] confidence A float number between 0 and 1 to indicate the confidence
     *                        level of the estimated throughput
     */
    uint32_t predictThroughput( AseTimeVal const&      now,
                                ThroughputHistoryPtr        pCurrentHistory,
                                uint32_t                    currentThroughput,
                                double&                     confidence );


    double getConfidence( AseTimeVal const& now );

    void connectTimeStat( AseTimeVal const& now, uint32_t& avgConnectTime, uint32_t& varConnectTime ) const;

    shared_ptr<IThroughputDistribution> getDistribution( AseTimeVal const& now ) const;

    void setNetworkInterfaceInfo( NetworkInterfaceInfo const& netinfo );

    void setClientExternalIpAddress( std::string const& ipaddr );

    void throughputMeasurement( uint32_t throughput, AseTimeVal const& now );

    void connectTimeMeasurement( AseTimeVal const& connectTime, AseTimeVal const& now );

    void addConnectionFailure( AseTimeVal when );

    AseTimeVal getLastHistoryUpdateTime() const;

    Variant getState() const;

    Variant getStateParams() const;

    void setState( Variant const& state );

protected:

    struct HistoryData;
    struct NetworkConnectionHistory;

    typedef shared_ptr<NetworkConnectionHistory> NetworkConnectionHistoryPtr;
    typedef std::string NetworkConnectionKey;

    static NetworkConnectionKey networkConnectionKey( NetworkInterfaceInfo const& netinfo );

    HistoryData* getHistoryData( AseTimeVal const& now ) const;

    shared_ptr<NetworkConnectionHistory> getHistoryForNetwork( NetworkConnectionKey const& key );

    int timeOfDayIndex( AseTimeVal const& now ) const;

    void setCurrentNetworkConnectionHistory(
                            NetworkConnectionKey const& key,
                            NetworkConnectionHistoryPtr pCurrentNetworkConnectionHistory );

protected:
    struct HistoryData
    {
        ThroughputDistributionPtr   mThroughputDistributionPtr;
        AseTimeVal             mLastDistributionUpdateTime;
        ullong                      mAverageConnectTime;
        ullong                      mAverageSquareConnectTime;

        HistoryData( ThroughputDistribution::Params const& tpparams );

        void configure( ThroughputDistribution::Params const& tpparams );

        void throughputMeasurement( uint32_t throughput, AseTimeVal const& now );
        void connectTimeMeasurement( AseTimeVal const& connectTime, double alpha );

        Variant getState() const;

        void setState( Variant const& value );

        HistoryData clone();    // deep copy
    };

    struct NetworkConnectionHistory
    {
        HistoryData                 mTimelessHistory;
        std::vector<HistoryData>    mTimeOfDayHistory;
        AseTimeVal             mLastConnectionFailureTime;

        NetworkConnectionHistory(   int timeOfDayGranularity,
                                    ThroughputDistribution::Params const& tpparams );

        void configure( int timeOfDayGranularity,
                        ThroughputDistribution::Params const& tpparams );

        Variant getState() const;

        void setState( Variant const& state );
    };

    typedef std::map<NetworkConnectionKey,NetworkConnectionHistoryPtr> ThroughputHistoryMap;

protected:
    int const                               mTimeOfDayGranularity;
    double const                            mConnectTimeAlpha;
    ThroughputDistribution::Params const    mThroughputDistributionParams;
    AseTimeVal                         mConnectionFailureExpirationTime;
    shared_ptr<ThroughputDistributionPredictor> mPredictor;

    ThroughputHistoryMap        mHistory;

    bool                        mHaveCurrentNetworkConnection;
    NetworkConnectionKey        mCurrentNetworkConnectionKey;
    NetworkConnectionHistoryPtr mCurrentNetworkConnectionHistory;
};

typedef ThroughputHistory::ThroughputHistoryPtr ThroughputHistoryPtr;

inline
int ThroughputHistory::timeOfDayIndex( AseTimeVal const& now ) const
{
    return now.reducedHours() / mTimeOfDayGranularity;
}

} // namespace ase
} // namespace netflix

#endif /* THROUGHPUT_HISTORY */
