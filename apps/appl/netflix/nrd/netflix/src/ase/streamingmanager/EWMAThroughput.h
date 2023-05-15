/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EWMA_THROUGHPUT_H
#define EWMA_THROUGHPUT_H

#include <nrdase/AseCommonDataTypes.h>
#include <nrdnet/AseTimeVal.h>

#define EWMA_THROUGHPUT_ALPHA 0.5f

namespace netflix {
namespace ase
{

/**
 * @class EWMAThroughput
 * @brief exponentially weighted moving average throughput
 */
class EWMAThroughput
{
public:
    EWMAThroughput(uint32_t maxTcpRecvBuffer);

    /** Set ID. */
    void setId( uint32_t id );

    /** Set the initial throughput. */
    void setInitialThroughput( ullong initialThroughput );

    /** Indicate bytes received
    *  @param count [in] The number of bytes received
    */
    void bytesReceived( ullong count );

    /** Indicate that reading from the socket has been paused
     * @param[in] ignoreBufferAtResume: whether to ignore data in tcp receive buffer
     *            when resuming downloading
    */
    void readingPaused( bool ignoreBufferAtResume);

    /** Get the current throughput in bps */
    ullong getThroughput();

//protected:

    /** use config parameters */
    AseTimeVal mSampleWindow;

    uint32_t mId;

    ullong mThroughput;

    AseTimeVal mSampleStartTime;
    AseTimeVal mSampleEndTime;
    ullong mTotalBytes;

    bool mInited;
    bool mPaused;

    AseTimeVal mPauseStartTime;

    ullong mMaxTcpRecvBuffer;
    ullong mSkippedBytes;
    bool     mIgnoreDataInBuffer;
};

}}
#endif
