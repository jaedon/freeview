/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef IMEDIA_CONSUMER_H
#define IMEDIA_CONSUMER_H

/** @file IMediaConsumer.h */

#include <vector>
#include <string>

#include <nrdbase/DataBuffer.h>

#include <nrdnet/AseUrl.h>
#include <nrdnet/AseTimeVal.h>
#include <nrdnet/NetCommonDataTypes.h>
#include <nrdnet/HttpResponseHeaders.h>

#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/AseErrorCodes.h>
#include <nrdase/ByteRange.h>
#include <nrdase/AseBuffer.h>

namespace netflix {
namespace ase {

//class ILocationSelector;
//typedef shared_ptr<ILocationSelector> ILocationSelectorPtr;

class IThroughputMonitor;
typedef shared_ptr<IThroughputMonitor> IThroughputMonitorPtr;

class IAseConfigParameter;
typedef shared_ptr<IAseConfigParameter> IAseConfigParameterPtr;

/**
 * @class MediaRequest
 * @brief Base class for an object representing a media request
 */
class MediaRequest
{
    public:
        /* Status codes returned by methods of this class */
        //static const int STATUS_CANCEL_REQUEST = -1;    // Indicates that this request should be cancelled and no futher calls made on this interface
        //static const int STATUS_TRIGGER_REQUEST = -2;   // Indicates that a new IMediaConsumer GetNextRequest should be triggered

        /** Shared pointer to an object instance implementing this interface */
        typedef shared_ptr<MediaRequest> MediaRequestPtr;

        virtual ~MediaRequest() {}

        /** Get the media type for this request */
        //MediaType getMediaType() const;

        /** Get the media track id associated with the media request */
        uint32_t getDownloadTrackId() const;

        /** Set the media track id associated with the media request */
        void setDownloadTrackId(uint32_t trackId);

        /** Get the stream key. */
        //const std::string& getStreamKey() const;

        /** Set the stream key */
        //void setStreamKey(std::string const& streamKey);

        /** set whether it's an open range request */
        void setOpenRangeRequest(bool openRange);

        /** Whether the it is an open range request */
        bool openRangeRequest() const;

        /** Whether this is a headers request */
        //bool headersRequest() const;

        /** Has this request been marked as abandoned */
        bool isAbandoned() const;

        /** Set the abandoned flag */
        void setAbandoned();

        uint32_t getRequestId() const;
        void setRequestId(uint32_t requestId);
        /*===== Interface to be implemented by subclasses =====*/

        /** Get the URLs for the request */
        //virtual ManifestUrls const& getUrls() const = 0;

        /** Get the byte range for the request */
        virtual ByteRange const& getByteRange( ) const = 0;

        /** Get the time at which this request should be completed */
        virtual AseTimeVal getCompletionDeadline() const;

        /** Get the location selector to use for this and subsequent requests */
        //virtual ILocationSelectorPtr getLocationSelector() const;

        /** Set the location selector to use for this and subsequent requests */
        //virtual void setLocationSelector(ILocationSelectorPtr selector);

        /** Get the size of response body buffers
         * For example, for IMediaBuffer, it is the size of a buffer block.
         */
        virtual size_t getBufferSize() const = 0;

        /** Get the progress interval in bytes, zero indicating no progress required */
        virtual size_t getProgressInterval() const;

        /** Get a buffer for the response body - all provided buffers
         *  must have the same size
         */
        virtual AseErrorCode getResponseBodyBuffer(AseBufferPtr& pBuffer) = 0;

        /** Indicate first request started
         *  @param[in] connecting true if a new connection is being established
         *  @param[in] pipelined true if the request is pipelined on an existing connection
         */
        virtual void requestStarted( bool connecting, bool pipelined ) = 0;

        /** Indicate a fatal request error */
        virtual void requestError( AseErrorCode status, uint32_t httpCode ) = 0;

        /** Provide the MEDIA response body
         *
         *  @param[in] pBuffer  The buffer that contains the response
         *                      body data. This may not contain the
         *                      complete body, but will be full if this
         *                      is not the last portion of the response.
         *  @param[in] sequence The sequence number of the provided buffer
         *                      (body data may not be provided in sequence)
         *  @param[out] abandontime The abandon timeout - the request will be abandoned if no
         *                      more data is received before this time
         *  @return: negative error codes; 1 if the request is completed
         */
        virtual int provideResponseBody(    AseBufferPtr const& pBuffer,
                                            int                 sequence,
                                            AseTimeVal&    abandontime ) = 0;

        /** Report progress */
        virtual void reportProgress( size_t bytesReceived );

        /** Indicate that the file size has been determined */
        virtual void provideFileSize( ullong size ) = 0;

        /** Indicate that the request was abandoned */
        virtual void abandoned() = 0;

        /** Provide the HTTP response headers */
        virtual void provideResponseHeaders( HttpResponseHeaders headers );

        /** Report http response time */
        virtual void reportHttpResponseTime( AseTimeVal const& responseTime );

        /** Return true if this request has a URL assigned */
        virtual bool hasUrl() const = 0;

        /** Select the URL and return true if it has changed */
        virtual bool selectUrl() = 0;

        /** Get the URL for this request */
        virtual AseUrl getCurrentUrl() = 0;

        /** Get the throughput monitor for this request - if any */
        virtual IThroughputMonitorPtr getThroughputMonitor() const;

        //virtual AseErrorCode getRequestData(DataBuffer& data) = 0;

        virtual void loadStarted() = 0;

protected:
        MediaRequest();

protected:
        uint32_t        		mDownloadTrackId;
        uint32_t                mRequestId;

        bool            		mOpenRangeRequest;
        bool            		mAbandoned;
};

typedef MediaRequest::MediaRequestPtr MediaRequestPtr;

/**
 * @class IMediaConsumer
 * @brief Interface for classes that wish to generate MEDIA requests and receive responses
 */
class IMediaConsumer : public enable_shared_from_this<IMediaConsumer>
{
public:
    typedef shared_ptr<IMediaConsumer> IMediaConsumerPtr;

    /** Media consumer state. */
    typedef enum
    {
        MEDIA_CONSUMER_STOPPED_STATE = 0,
        MEDIA_CONSUMER_BUFFERING_STATE,
        MEDIA_CONSUMER_PLAYING_STATE,
        MEDIA_CONSUMER_REBUFFERING_STATE
    } State;

public:
    virtual ~IMediaConsumer() {}

    virtual void configure( const IAseConfigParameterPtr &pConfig ) = 0;

    virtual void performStreamingTasks() = 0;

    /** Get a new Request object
     *  @param downloadTrackId [in] The download track id for which a request is needed
     *  @param pRequest [out] The new MediaRequest object, which may be for a different track
     *                        if the consumer wants to trigger earlier processing by that track
     *  @return 0   for success, negative error code.
     */
    virtual AseErrorCode getNextRequest(    uint32_t            downloadTrackId,
                                            MediaRequestPtr&    pRequest ) = 0;

    /** Get the buffer statistics
     * @param mediaType [in] the media type
     * @param usedBuffer [out] the current used buffer size in bytes
     * @param availableBuffer [out] the avaialable buffer size in bytes
     */
    virtual void getBufferStats( MediaType mediaType,
                                 ullong& usedBuffer, ullong& availableBuffer ) = 0;


    /** Check whether the buffer is available */
    virtual bool isBufferAvailable(MediaType mediaType) = 0;

    /**
     * report that pipelining as disabled
     */
    virtual void pipeliningDisabled() = 0;

    /**
     * Report non-fatal network errors
     * This is called periodically when the HRM is observing failures
     * It will be called with latestStatus = AS_NO_ERROR when data is received again
     * @param latestStatus The error code of the last failure, or AS_NO_ERROR if data is received
     * @param latestUrl The url on which that failure occured
     * @param latestErrorTime The time at which the latest error occured
     * @param latestDataReceived The last time data was received
     */
    virtual void networkFailing(    uint32_t                dwonloadTrackId,
                                    AseErrorCode            latestStatus,
                                    AseUrl::HostPort const& destination,
                                    AseTimeVal const*       pLatestErrorTime,
                                    AseTimeVal const&       latestDataReceived ) = 0;

    /**
     * Get the state name.
     */
    static const char* getStateName( State state );

};

typedef IMediaConsumer::IMediaConsumerPtr IMediaConsumerPtr;

inline MediaRequest::MediaRequest()
    : mDownloadTrackId( 0 )
    , mRequestId(0)
    , mOpenRangeRequest( false )
    , mAbandoned( false )
{
}

inline bool MediaRequest::openRangeRequest() const
{
    return mOpenRangeRequest;
}

inline void MediaRequest::setOpenRangeRequest(bool openRangeRequest)
{
    mOpenRangeRequest = openRangeRequest;
}

#if 0
inline MediaType MediaRequest::getMediaType() const
{
    return getUrls().front()->getParent()->getParent()->getMediaType();
}
#endif

inline uint32_t MediaRequest::getDownloadTrackId() const
{
    return mDownloadTrackId;
}

inline void MediaRequest::setDownloadTrackId( uint32_t trackId )
{
    mDownloadTrackId = trackId;
}

inline uint32_t MediaRequest::getRequestId() const
{
    return mRequestId;
}

inline void MediaRequest::setRequestId(uint32_t requestId)
{
    mRequestId = requestId;
}

inline bool MediaRequest::isAbandoned() const
{
    return mAbandoned;
}

inline void MediaRequest::setAbandoned()
{
    mAbandoned = true;
}

inline void MediaRequest::reportHttpResponseTime( AseTimeVal const& /*responseTime*/ )
{
}

inline void MediaRequest::provideResponseHeaders( HttpResponseHeaders /*headers*/ )
{
}

#if 0
inline std::string const& MediaRequest::getStreamKey() const
{
    return mStreamKey;
}

inline void MediaRequest::setStreamKey(std::string const& streamKey)
{
    mStreamKey = streamKey;
}

inline bool MediaRequest::headersRequest() const
{
    return mHeaderRequest;
}
#endif

inline AseTimeVal MediaRequest::getCompletionDeadline() const
{
    return AseTimeVal::INFINITE;
}


#if 0
inline ILocationSelectorPtr MediaRequest::getLocationSelector() const
{
    return mLocationSelector;
}

inline void MediaRequest::setLocationSelector(ILocationSelectorPtr selector)
{
    mLocationSelector = selector;
}
#endif

inline size_t MediaRequest::getProgressInterval() const
{
    return 0;
}

inline void MediaRequest::reportProgress( size_t /* bytesReceived */ )
{
}

inline IThroughputMonitorPtr MediaRequest::getThroughputMonitor() const
{
    return IThroughputMonitorPtr();
}

inline const char* IMediaConsumer::getStateName( IMediaConsumer::State state )
{
    const char* pRetVal = "Unknown";

    switch (state)
    {
        case MEDIA_CONSUMER_STOPPED_STATE:
        {
            pRetVal = "Stopped";
            break;
        }

        case MEDIA_CONSUMER_BUFFERING_STATE:
        {
            pRetVal = "Buffering";
            break;
        }

        case MEDIA_CONSUMER_PLAYING_STATE:
        {
            pRetVal = "Playing";
            break;
        }

        case MEDIA_CONSUMER_REBUFFERING_STATE:
        {
            pRetVal = "ReBuffering";
            break;
        }
    }

    return pRetVal;
}

}}
#endif /* IMEDIA_CONSUMER_H */
