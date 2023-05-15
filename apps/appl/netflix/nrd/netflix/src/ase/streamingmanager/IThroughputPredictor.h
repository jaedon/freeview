/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef I_THROUGHPUT_PREDICTOR_H
#define I_THROUGHPUT_PREDICTOR_H

#include <nrdnet/AseTimeVal.h>
#include <nrdase/AseCommonDataTypes.h>

#include <nrdbase/Variant.h>
#include <nrdbase/Time.h>

#include <string>

namespace netflix {
namespace ase {

class IThroughputDistribution;

/**
 * @class IThroughputPredictor
 * @brief: Base class for classes which can predict throughput
 */
class IThroughputPredictor : public enable_shared_from_this<IThroughputPredictor>
{
public:
    typedef shared_ptr<IThroughputPredictor> IThroughputPredictorPtr;

    virtual ~IThroughputPredictor()
    {
    }

    /**
     * Obtain the predicted throughput from historical data
     * @param[in] pCurrentThroughputPredictor the current throughput predictor
     * @param[in] current The current short-term throughput, zero if not known
     * @param[in] current buffer level
     * @param[out] confidence: a float number between 0 and 1 to indicate the confidence
     *                         level of the estimated throughput
     */
    virtual uint32_t predictThroughput( IThroughputPredictorPtr     pCurrentThroughputPredictor,
                                        uint32_t                    current,
                                        uint32_t                    currentBufferLevel,
                                        double&                     confidence ) = 0;

    /**
     * get the confidence value for the historical information
     */
    virtual double getConfidence() = 0;

    /**
     * Get the name of the predictor for logging purposes
     */
    virtual std::string const& name() const = 0;
};

typedef IThroughputPredictor::IThroughputPredictorPtr IThroughputPredictorPtr;

} // namespace ase
} // namespace netflix


#endif /* I_THROUGHPUT_PREDICTOR_H */
