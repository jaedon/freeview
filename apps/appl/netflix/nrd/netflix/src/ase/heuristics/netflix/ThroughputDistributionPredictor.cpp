/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ThroughputDistributionPredictor.h"
#include "ThroughputDistribution.h"

#include <nrdbase/tr1.h>
#include <limits>

using namespace netflix::ase;
using namespace std;

ThroughputDistributionPredictor::Params::Params()
    : mPredictionMethod( FIXED_QUANTILE )
    , mPredictionQuantile( 0.25 )
    , mCapPredictionAtCurrent( true )
    , mLocationEstimateMargin( 25 )
    , mMinimumConfidence( 0.2 )
    , mConfidenceFactor( 1.0 )
{
}

void ThroughputDistributionPredictor::Params::initialize( IAseConfigParameterPtr pConfigParameters )
{
    int const intmax = std::numeric_limits<int32_t>::max();

    std::string const predictionMethodStr = pConfigParameters->queryParameter( "predictionMethod" );

    mPredictionMethod = ( predictionMethodStr == "highestmode" ) ? HIGHEST_MODE
                                        : ( predictionMethodStr == "fixedquantile" ) ? FIXED_QUANTILE
                                        : ( predictionMethodStr == "likelihoodratio" ) ? LIKELIHOOD_RATIO
                                        : FIXED_QUANTILE;

    mPredictionQuantile = pConfigParameters->getParameterAs<double>( "predictionQuantile", 0.0, 1.0, 0.25 );

    mCapPredictionAtCurrent = pConfigParameters->getParameterAs<bool>( "capPredictionAtCurrent", true );

    mLocationEstimateMargin = pConfigParameters->getParameterAs<uint32_t>( "locationEstimateMargin", 25 );

    mMinimumConfidence = pConfigParameters->getParameterAs<double>( "throughputLowestConfidenceLevel", 0.2);

    int32_t maxConfidenceTime = pConfigParameters->getParameterAs<int32_t>( "maxConfidenceTime", 1, intmax, 0 ) * 1000;

    // 12.4 name for the above configuration param, included here for compatibility
    int32_t const minMeasurementTime = pConfigParameters->getParameterAs<int32_t>( "minMeasurementTime", 1, intmax, 600 ) * 1000;

    if ( maxConfidenceTime == 0 )
        maxConfidenceTime = minMeasurementTime;

    int32_t const effectiveCountHalflife = pConfigParameters->getParameterAs<int32_t>( "measurementHalflife", 1, intmax, 10 * 3600 ) * 1000;

    int32_t const distributionUpdateInterval = pConfigParameters->getParameterAs<int32_t>( "distributionUpdateInterval", 100, intmax, 2000 );

    double const maxEffectiveCount = ThroughputDistribution::effectiveCountForActiveTime(
                                                effectiveCountHalflife,
                                                distributionUpdateInterval,
                                                maxConfidenceTime );

    mConfidenceFactor = 1.0 - 1.0 / sqrt( maxEffectiveCount );
}

ThroughputDistributionPredictor::ThroughputDistributionPredictor( ThroughputDistributionPredictor::Params const& params )
    : mParams( params )
{
}

void ThroughputDistributionPredictor::configure( Params const& params )
{
    mParams = params;
}

uint32_t ThroughputDistributionPredictor::predictThroughput(
                                    IThroughputDistribution const&  distribution,
                                    IThroughputDistribution const*  pCurrentDistribution,
                                    uint32_t                        currentThroughput,
                                    double&                         confidence ) const
{
    AseTimeVal const now = AseTimeVal::now();

    double predictionConfidence = 0.0;
    uint32_t prediction = 0;
    uint32_t throughput = 0;

    switch( mParams.mPredictionMethod )
    {
        case FIXED_QUANTILE:
            prediction = quantilePrediction( distribution );
            break;
        case HIGHEST_MODE:
            prediction = highestModePrediction( distribution, mParams.mCapPredictionAtCurrent ? currentThroughput : 0 );
            break;
        case LIKELIHOOD_RATIO:
            if ( pCurrentDistribution )
            {
                prediction = likelihoodRatioPrediction( distribution, pCurrentDistribution, currentThroughput );
            }
            else
            {
                prediction = quantilePrediction( distribution );
            }
            break;
    }

    if ( prediction > 0 )
    {
        // decide on the confidence
        predictionConfidence = ( 1.0 - 1.0 / sqrt( distribution.effectiveCount( now ) ) ) / mParams.mConfidenceFactor;

        predictionConfidence =    ( predictionConfidence < mParams.mMinimumConfidence ) ? mParams.mMinimumConfidence
                                : ( predictionConfidence > 1.0 ) ? 1.0
                                : predictionConfidence;
    }

    if ( ( currentThroughput == 0                           // no current measurement
        || prediction < currentThroughput                   // our prediction is lower than current anyway
        || !mParams.mCapPredictionAtCurrent )                       // don't cap at current anyway
        && ( predictionConfidence >= mParams.mMinimumConfidence ) ) // confidence >= min( min confidence, current confidence)
    {
        // use our prediction
        confidence = predictionConfidence;
        throughput = prediction;
    }
    else if ( predictionConfidence > 0 )
    {
        // we don't have any helpful history, but we do have some - just use the current measurement
        confidence = 1.0;
        throughput = currentThroughput;
    }
    else
    {
        // use a discounted, low confidence version of current
        confidence = mParams.mMinimumConfidence * 1.2;
        throughput = (100 - mParams.mLocationEstimateMargin) * currentThroughput / 100;
    }

    return throughput;
}

double ThroughputDistributionPredictor::getConfidence( IThroughputDistribution const& distribution ) const
{
    double const effectiveCount = distribution.effectiveCount( AseTimeVal::now() );
    if ( effectiveCount == 0 )
        return 0.0;

    double predictionConfidence = ( 1.0 - 1.0 / sqrt( effectiveCount ) ) / mParams.mConfidenceFactor;

    return ( predictionConfidence < mParams.mMinimumConfidence ) ? mParams.mMinimumConfidence
                                : ( predictionConfidence > 1.0 ) ? 1.0
                                : predictionConfidence;
}

int ThroughputDistributionPredictor::quantilePrediction( IThroughputDistribution const&  distribution ) const
{
    // This algororithm predicts based on a fixed quantile in the distribution
    return distribution.invcdf( mParams.mPredictionQuantile );
}

int ThroughputDistributionPredictor::highestModePrediction( IThroughputDistribution const&  distribution, int current ) const
{
    // This algorithm predicts based on the highest mode of the target distribution which
    // is less than or equal to the current observation
    int const throughput = distribution.highestmode( current );
    if ( throughput == 0 )
    {
        // Not all IThroughputDistribution classes support modes
        return quantilePrediction( distribution );
    }

    return throughput;
}

int ThroughputDistributionPredictor::likelihoodRatioPrediction( IThroughputDistribution const&  distribution,
                                                                IThroughputDistribution const*  pCurrentDistribution,
                                                                int currentThroughput ) const
{
    // This algorithm looks at the probability of the current measurement in the current history
    // and compares with the probability on the target distribution
    double const current_prob = 1.0 - pCurrentDistribution->cdf( currentThroughput );
    double const target_prob = 1.0 - distribution.cdf( currentThroughput );

    if ( current_prob > target_prob )
    {
        // We're more likely currently to get this measurement or higher than on the target
        // so we return the highest mode not greater than the current measurement
        return highestModePrediction( distribution, currentThroughput );
    }
    else
    {
        // We're more likely to get this measurement on the target than globally
        // so just return this measurement
        return currentThroughput;
    }
}
