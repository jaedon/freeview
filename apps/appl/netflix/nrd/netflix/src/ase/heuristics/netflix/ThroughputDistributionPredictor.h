/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef THROUGHPUT_DISTRIBUTION_PREDICTOR_H
#define THROUGHPUT_DISTRIBUTION_PREDICTOR_H

#include <nrdase/DebugLogging.h>
#include <nrdase/IAseConfigParameter.h>
#include <nrdase/IThroughputPredictor.h>

namespace netflix {
namespace ase {

DECLARE_LOGGER( LocationSetMonitorLog );

class IThroughputDistribution;

/** This class makes throughput predictions based on probability distributions
 *  represented by the IThroughputDistribution interface
 */
class ThroughputDistributionPredictor
{
public:
    typedef shared_ptr<ThroughputDistributionPredictor> ThroughputDistributionPredictorPtr;

public:
    enum PredictionMethod
    {
        FIXED_QUANTILE = 0,
        HIGHEST_MODE,
        LIKELIHOOD_RATIO
    };

    struct Params
    {
        PredictionMethod      mPredictionMethod;
        double                mPredictionQuantile;
        bool                  mCapPredictionAtCurrent;
        uint32_t              mLocationEstimateMargin;
        double                mMinimumConfidence;
        double                mConfidenceFactor;

        Params();

        void initialize( IAseConfigParameterPtr pConfigParameters );
    };

    ThroughputDistributionPredictor( Params const& params );

    void configure( Params const& params );

    /**
     * obtain the predicted throughput from historical data
     * @param[in] current The current short-term throughput
     * @param[in] pAggregateDistribution Throughput predictor for aggregate including this predictor
     * @param[out] confidence: a float number between 0 and 1 to indicate the confidence
     *                         level of the estimated throughput
     */
    uint32_t predictThroughput( IThroughputDistribution const&  distribution,
                                IThroughputDistribution const*  pCurrentDistribution,
                                uint32_t                        currentThroughput,
                                double&                         confidence ) const;

    double getConfidence( IThroughputDistribution const&  distribution ) const;

    Params const& getParams() const;

protected:

    int quantilePrediction( IThroughputDistribution const&  distribution ) const;

    int highestModePrediction( IThroughputDistribution const&  distribution, int current ) const;

    int likelihoodRatioPrediction(  IThroughputDistribution const&  distribution,
                                    IThroughputDistribution const*  pCurrentDistribution,
                                    int currentThroughput ) const;
protected:

    Params mParams;
};

typedef ThroughputDistributionPredictor::ThroughputDistributionPredictorPtr ThroughputDistributionPredictorPtr;

inline ThroughputDistributionPredictor::Params const&
ThroughputDistributionPredictor::getParams() const
{
    return mParams;
}

} // namespace ase
} // namespace netflix

#endif
