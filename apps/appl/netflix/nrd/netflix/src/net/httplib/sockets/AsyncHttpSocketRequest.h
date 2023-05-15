/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASYNC_HTTP_SOCKET_REQUEST_H
#define ASYNC_HTTP_SOCKET_REQUEST_H

#include <nrdnet/IAsyncHttpLib.h>
#include <nrdbase/tr1.h>

namespace netflix {

class AsyncHttpSocketConnection;

/**
 * @class
 * @brief
 */
class AsyncHttpSocketRequest : public IAsyncHttpRequest
{
public:
    typedef shared_ptr<AsyncHttpSocketRequest> AsyncHttpSocketRequestPtr;

    /**
     *  @enum ChunkedTransferState
     *
     *  Chunked transfer states.
     */
    typedef enum
    {
        RECEIVING_CHUNK_SIZE_STATE = 0,
        RECEIVING_CHUNK_DATA_BODY_STATE,
        RECEIVING_CHUNK_DATA_END_STATE,
        RECEIVING_CHUNK_TRAILER_STATE
    } ChunkedTransferState;

public:
    /** Constructor. */
    AsyncHttpSocketRequest(
            AsyncHttpSocketConnection* pAsyncHttpSocketConnection,
            uint32_t id,
            Method method,
            Version version,
            IHttpRequestTraceListenerPtr pTraceListener );

    /** Destructor. */
    virtual ~AsyncHttpSocketRequest();

    /** Set the state. */
    virtual void setState( State state );

    /** Set the request URL. */
    virtual void setRequestUrl( AseUrl const& url );

    /** Get the request URL. */
    virtual AseUrl const& getRequestUrl() const;

    /** Set the request headers. */
    virtual void setRequestHeaders( HttpHeaders const& headers );

    /** Get the request headers. */
    virtual HttpHeaders const& getRequestHeaders() const;

    /** Set the request body. */
    virtual void setRequestBody(
                        unsigned char* pBodyData,
                        uint32_t bodyLength );

    /** Get the request body. */
    virtual void getRequestBody(
                        unsigned char*& pBodyData,
                        uint32_t& bodyLength ) const;

    /** Set the timeout duration. */
    virtual void setTimeoutDuration(
                        const AseTimeVal& timeoutDuration );

    /** Get the timeout duration. */
    virtual const AseTimeVal& getTimeoutDuration() const;

    /** Encode the request into the sending staging buffer.
     *
     *  @param pBuffer [In] The buffer to store the encoded request.
     *  @param bufferLength [In/Out] When input, it is the capacity of the buffer.
     *                               If the output bufferLength is 0, that means
     *                               there is no more data to encode.
     *                               When output, it is the size of encoded request.
     */
    virtual void encodeRequest(
                        unsigned char* pBuffer,
                        uint32_t& bufferLength );

    /** Set the response code. */
    virtual void setResponseStatus(
                        const std::string& httpVersion,
                        const std::string& code,
                        const std::string& reasonPhrase );

    /** Add a response header name. */
    virtual void addResponseHeaderName(
                        const unsigned char* pNameData,
                        uint32_t nameLength );

    /** Add a response header value.
     *
     *  This function may be called multiple times after a addResponseHeaderName(). In
     *  this case, the multiple value data are to be concatenated.
     */
    virtual void addResponseHeaderValue(
                        const unsigned char* pValueData,
                        uint32_t valueLength );

    /** Signal that a response header has been fully received and parsed. */
    virtual int32_t signalResponseHeaderCompleted();

    /** Set the response body to size to be zero for the case where this is not clear from the header */
    virtual void setResponseBodyEmpty();

    /** Add staged response body. When AsyncHttpSocketConnection reads HTTP response
     *  headers out of a socket into the receiving staging buffer, it is possible that
     *  part of the response body is also read. This part of response body is considered
     *  "staged". This function is to notify AsyncHttpSocketRequest about the staged
     *  response body, so that when AsyncHttpSocketRequest::getResponseBody() is called,
     *  the staged body response body can be properly returned.
      */
    virtual void addStagedBody(
                        const unsigned char* pStagedBodyData,
                        uint32_t stagedBodyLength );

    /** Added staged chunks.
     *
     *  stagedChunkLength is the actual availabe length of the chunk data.
     *  stagedChunkSize is the size of the chunk data.
     *
     *  stagedChunkSize >= stagedChunkLength
     */
    virtual void addStagedChunk(
                        const unsigned char* pStagedChunk,
                        uint32_t stagedChunkLength,
                        uint32_t stagedChunkSize );

    /** Check if there is staged response body.. */
    virtual bool hasStagedResponseBody();

    /** Set the chunked transfer state. */
    virtual void setChunkedTransferState( ChunkedTransferState state );

    /** Get the chunked transfer state. */
    virtual ChunkedTransferState getChunkedTransferState() const;

    /** Receive the regular response body. */
    virtual int32_t receiveRegularResponseBody( unsigned char* pBuffer, uint32_t& bufferLength );

    /** Receive the chunked response body. */
    virtual int32_t receiveChunkedResponseBody( unsigned char* pBuffer, uint32_t& bufferLength );

    /** Indicate bytes received */
    virtual void indicateHeaderBytesReceived(
                                AseTimeVal const& time,
                                uint32_t bytes,
                                bool complete = false );

    /** Indicate receiving is paused on this request. */
    virtual void pauseReceiving( const AseTimeVal& now );

    /** Indicate receiving is resumed on this request. */
    virtual void resumeReceiving( const AseTimeVal& now );

    /** Set the timeout time. */
    virtual void setTimeoutTime( const AseTimeVal& now );

    /** IAsyncHttpRequest functions. */

    /** Get the ID. */
    virtual uint32_t getId() const;

    /** Get the method. */
    virtual Method getMethod() const;

    /** Get the version. */
    virtual Version getVersion() const;

    /** Get the state */
    virtual State getState() const;

    /** Get the progress of the request. The progress is defined as the percentage
     *  of the received body length over the requested data length. In the case
     *  where the request data length is unknown, the progress is also unknown.
     *
     *  @return 0 ~ 100
     *          -1 if unknown.
     */
    virtual int32_t getProgress() const;

    /** Set the user data which is some opaque data which the caller wants to associate with the request. */
    virtual void setUserData( void* pData );

    /** Get the user data. */
    virtual void* getUserData() const;

    /** Get the response HTTP version, only valid when state >= RECEIVING_HEADERS_STATE.
     *
     *  @param major [Out] The HTTP major version.
     *  @param minor [Out] The HTTP minor version.
     */
    virtual void getResponseHttpVersion( uint32_t& major, uint32_t& minor );

    /** Get the response code, only valid when state >= RECEIVING_DATA_STATE */
    virtual uint32_t getResponseCode() const;

    /** Get the value of a particular response header, only valid when state >= RECEIVING_DATA_STATE */
    virtual std::string getResponseHeaderValue( std::string const& name ) const;

    /** Get all the response headers, only valid when state >= RECEIVING_DATA_STATE */
    virtual HttpResponseHeaders getResponseHeaders() const;

    /** Get the chunked transfer flag. */
    virtual bool getChunkedTransferFlag() const;

    /** Get the content length. */
    virtual llong getContentLength() const;

    /** Get the response body length. */
    virtual llong getResponseBodyLength() const;

    /** Get the response body, only valid when state >= RECEIVING_DATA_STATE.
     *
     *  This function may not return the whole body. It is possible that this function returns 0 or 1,
     *  but the returned buffer length is 0. This may happen, for example, in the case of chunked
     *  transfer as shown below.
     *
     *  B
     *  First chunk      ----> getResponseBody() returns 0 with buffer length set to 11
     *  C
     *  Second chunk     ----> getResponseBody() returns 0 with buffer length set to 12
     *  0                ----> getResponseBody() returns 0 with buffer length set to 0
     *  MD5: 0xabcdefg   ----> getResponseBody() returns 1 with buffer length set to 0
     *
     *  If this function returns a connection failure error, the caller should close
     *  the connection and assume all the outstanding requests are lost.
     *
     *  @param pBuffer [In] The buffer to store the response body.
     *  @param bufferLength [In/Out] When input, it is the capacity of the buffer.
     *                               When outout, it is the size of response body read.
     *
     *  @return 1 on success and the complete body has been read
     *          AS_NO_ERROR on success
     *          AS_UNKNOWN_ERROR unknown error
     *          AS_CONNECTION_ERROR connection failures
     *          or other negative error codes.
     */
    virtual int32_t getResponseBody( unsigned char* pBuffer, uint32_t& bufferLength );

    /** As getResponseBody except that the body data is discarded
     *  @param bytesDiscarded [in/out] On entry, if limit = true, the max number of bytes to discard
     *                                 on exit, the number of bytes discarded
     *  @param limit [in] If true, at most bytesDiscarded bytes are discarded, otherwise all the
     *                    received body data so far will be discarded
     */
    virtual int32_t discardResponseBody( uint32_t& bytesDiscarded, bool const limit = false );

    /** Get the timeout time. */
    virtual const AseTimeVal& getTimeoutTime() const;

    /** Get the time the client sends the first byte. */
    virtual const AseTimeVal& getFirstByteSentTime() const;

    /** Get the time the client receives the first byte. */
    virtual const AseTimeVal& getFirstByteReceivedTime() const;

    /** Validate the message length. */
    virtual bool validMessageLength();

    void setMaxContentLength(llong maxMessageLength) { mMaxContentLength = maxMessageLength; }
    llong maxContentLength() const { return mMaxContentLength; }

protected:
    /** The parent AsyncHttpSocketConnection pointer. */
    AsyncHttpSocketConnection* mAsyncHttpSocketConnection;

    /** The ID of the request. */
    uint32_t mId;

    /** The method of the request. */
    Method mMethod;

    /** The version of the request. */
    Version mVersion;

    /** The state. */
    State mState;

    /** The time the request is created. */
    AseTimeVal mCreatedTime;

    /** The timeout time. */
    AseTimeVal mTimeoutTime;

    /** The time the first byte is sent out. */
    AseTimeVal mFirstByteSentTime;

    /** The time the first byte is received. */
    AseTimeVal mFirstByteReceivedTime;

    /** The time the receiving is paused. */
    AseTimeVal mPausedTime;

    /** The user data. */
    void* mUserDataPtr;

    /** The request URL. */
    AseUrl mRequestUrl;

    /** The request line. */
    std::string mRequestLine;

    /** The index of next char in the request line to be encoded. */
    std::string::size_type mEncodingRequestLinePos;

    /** The request headers. These include headers provided by the caller and automartically
     *  generated headers, e.g., the Host header.
     */
    HttpHeaders mRequestHeaders;

    /** The request body data. */
    unsigned char* mRequestBodyPtr;

    /** The request body length. */
    uint32_t mRequestBodyLength;

    /** The timeout duration. */
    AseTimeVal mTimeoutDuration;

    /** The connect timeout. */
    AseTimeVal mConnectTimeout;

    /** The iterator to the request header that is being encoded. When a request
     *  is sent out, it is first encoded and put in the sending staging buffer.
     *  If the staging buffer does not have enough space, remaining headers are encoded and
     *  put in the sending staging buffer after the current headers in the sending staging
     *  buffer have been sent. This iterator is pointing to the first remaining header.
     */
    HttpHeaders::iterator mEncodingRequestHeaderItr;

    /** The flag shows that the request headers end "\r\n" has been encoded.*/
    bool mRequestHeadersEndEncoded;

    /** The response HTTP version. */
    uint32_t mResponseHttpVersionMajor;
    uint32_t mResponseHttpVersionMinor;

    /** The response code. */
    uint32_t mResponseCode;

    /** The response headers. */
    HttpResponseHeaders mResponseHeaders;

    /** The iterator to the response header that is being added. */
    HttpResponseHeaders::iterator mAddingResponseHeaderItr;

    /** The staged response body data. */
    const unsigned char* mStagedBodyDataPtr;

    /** The staged response body length. */
    uint32_t mStagedBodyLength;

    /** The response body length, -1 if unknown. */
    llong mResponseBodyLength;

    /** The length of the response body that has been received. */
    llong mReceivedResponseBodyLength;

    /** The chunked transfer coding variables. */

    /** The flag that shows the chunked transfer coding is used to send the response body. */
    bool mChunkedTransferFlag;

    /** The chunked transfer state. */
    ChunkedTransferState mChunkedTransferState;

    /** The completed chunks that are in staging buffer. */
    std::list<std::pair<const unsigned char*, uint32_t> > mStagedCompletedChunkList;

    /** The iterator to the chunk being copied from the staing buffer. */
    std::list<std::pair<const unsigned char*, uint32_t> >::iterator mCopyingStagedChunkItr;

    /** The length of the ncopied data in the chunk beging copies. */
    uint32_t mCopiedChunkLength;

    /** The chunk data that is partially in the staging buffer. */
    const unsigned char* mStagedPartialChunkPtr;

    /** The length of the chunk data that is partially in the staging buffer. */
    uint32_t mStagedPartialChunkLength;

    /** The size of the chunk data that is partially in the staging buffer. */
    uint32_t mStagedPartialChunkSize;

    /** The HTTP request trace listener. */
    weak_ptr<IHttpRequestTraceListener> mTraceListenerPtr;

    llong mMaxContentLength;
};

typedef AsyncHttpSocketRequest::AsyncHttpSocketRequestPtr AsyncHttpSocketRequestPtr;

}
#endif //ASYNC_HTTP_LIB_SOCKET_H
