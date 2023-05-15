/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <assert.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>

#include <nrdbase/AseErrorCodes.h>
#include "AsyncHttpSocketConnection.h"
#include "AsyncHttpSocketRequest.h"

namespace netflix {

/**
 * @class
 * @brief
 */

/** Constructor. */
AsyncHttpSocketRequest::AsyncHttpSocketRequest(
                            AsyncHttpSocketConnection* pAsyncHttpSocketConnection,
                            uint32_t id,
                            Method method,
                            Version version,
                            IHttpRequestTraceListenerPtr pTraceListener ) :
                                    mAsyncHttpSocketConnection( pAsyncHttpSocketConnection ),
                                    mId( id ),
                                    mMethod( method ),
                                    mVersion( version ),
                                    mState( AsyncHttpSocketRequest::CREATED_STATE ),
                                    mCreatedTime( AseTimeVal::now() ),
                                    mFirstByteSentTime( AseTimeVal::ZERO ),
                                    mFirstByteReceivedTime( AseTimeVal::ZERO ),
                                    mPausedTime( AseTimeVal::INFINITE ),
                                    mRequestBodyPtr( NULL ),
                                    mRequestBodyLength( 0 ),
                                    mTimeoutDuration( AseTimeVal::INFINITE ),
                                    mStagedBodyDataPtr( NULL ),
                                    mStagedBodyLength( 0 ),
                                    mResponseBodyLength( -1LL ),
                                    mReceivedResponseBodyLength( 0LL ),
                                    mChunkedTransferFlag( false ),
                                    mChunkedTransferState( RECEIVING_CHUNK_SIZE_STATE ),
                                    mCopiedChunkLength( 0 ),
                                    mStagedPartialChunkPtr( NULL ),
                                    mStagedPartialChunkLength( 0 ),
                                    mStagedPartialChunkSize( 0 ),
                                    mTraceListenerPtr( pTraceListener ),
                                    mMaxContentLength( 0L)
{
    mEncodingRequestHeaderItr = mRequestHeaders.end();

    mAddingResponseHeaderItr = mResponseHeaders.end();

    mStagedCompletedChunkList.clear();

    mCopyingStagedChunkItr = mStagedCompletedChunkList.end();
}

/** Destructor. */
AsyncHttpSocketRequest::~AsyncHttpSocketRequest()
{
}

/** Set the state. */
void AsyncHttpSocketRequest::setState( State state )
{
    if (state == RECEIVING_STATUS_STATE)
    {
        mFirstByteReceivedTime = AseTimeVal::now();
    }

    mState = state;
}

/** Set the request URL. */
void AsyncHttpSocketRequest::setRequestUrl( AseUrl const& url )
{
    mRequestUrl = url;
}

/** Get the request URL. */
AseUrl const& AsyncHttpSocketRequest::getRequestUrl() const
{
    return mRequestUrl;
}

/** Set the request headers. */
void AsyncHttpSocketRequest::setRequestHeaders( HttpHeaders const& headers )
{
    mRequestHeaders = headers;
}

/** Get the request headers. */
HttpHeaders const& AsyncHttpSocketRequest::getRequestHeaders() const
{
    return mRequestHeaders;
}

/** Set the request body. */
void AsyncHttpSocketRequest::setRequestBody(
                                        unsigned char* pBodyData,
                                        uint32_t bodyLength )
{
    mRequestBodyPtr = pBodyData;
    mRequestBodyLength = bodyLength;
}

/** Get the request body. */
void AsyncHttpSocketRequest::getRequestBody(
                                        unsigned char*& pBodyData,
                                        uint32_t& bodyLength ) const
{
    pBodyData = mRequestBodyPtr;
    bodyLength = mRequestBodyLength;
}

/** Set the timeout duration. */
void AsyncHttpSocketRequest::setTimeoutDuration(
                                        const AseTimeVal& timeoutDuration )
{
    mTimeoutDuration = timeoutDuration;
}

/** Get the timeout duration. */
const AseTimeVal& AsyncHttpSocketRequest::getTimeoutDuration() const
{
    return mTimeoutDuration;
}

static inline std::string createRequestLine(const AseUrl &url, AsyncHttpSocketRequest::Method method, AsyncHttpSocketRequest::Version version)
{
    std::ostringstream stream;
    stream << IAsyncHttpRequest::requestMethodString(method) << ' ';

    if (!url.GetPath().empty()) {
        stream << url.GetPath();
    } else {
        stream << "/";
    }

    if (url.HasQuery()) {
        stream << "?" << url.GetQuery();
    }

    if (url.HasFragment()) {
        stream << "#" << url.GetFragment();
    }

    const char *ver = 0;
    switch (version) {
    case AsyncHttpSocketRequest::HTTP_1_0: ver = " HTTP/1.0\r\n"; break;
    case AsyncHttpSocketRequest::HTTP_1_1: ver = " HTTP/1.1\r\n"; break;
    }
    assert(ver);
    stream << ver;
    return stream.str();
}


/** Encode the request into the sending staging buffer. */
void AsyncHttpSocketRequest::encodeRequest(unsigned char* pBuffer, uint32_t& bufferLength )
{
    uint32_t encodedLength = 0;

    if (mState == CREATED_STATE) {
        mRequestLine = createRequestLine(mRequestUrl, mMethod, mVersion);
        // Add some default headers if they are not present.
        if (mRequestHeaders.find("Host") == mRequestHeaders.end()) {
            if (mRequestUrl.HasPort()) {
                mRequestHeaders["Host"] = mRequestUrl.GetHost() + ":" + mRequestUrl.GetPort();
            } else {
                mRequestHeaders["Host"] = mRequestUrl.GetHost();
            }
        }
        if (mMethod == POST_METHOD) {
            if (mRequestBodyLength > 0) {
                char lengthHeader[32];

                snprintf(
                    lengthHeader,
                    32,
                    "%u",
                    mRequestBodyLength );

                if (mRequestHeaders.find( "Content-Length" ) == mRequestHeaders.end()) {
                    mRequestHeaders["Content-Length"] = lengthHeader;
                }
            }
        } else if (mRequestHeaders.find("Accept") == mRequestHeaders.end()) {
            mRequestHeaders["Accept"] = "*/*";
        }

        mEncodingRequestLinePos = 0;
        mEncodingRequestHeaderItr = mRequestHeaders.begin();
        mRequestHeadersEndEncoded = false;
        mFirstByteSentTime = AseTimeVal::now();
        mState = SENDING_LINE_HEADERS_STATE;
    }

    //Encode the request line.
    if (mEncodingRequestLinePos != std::string::npos)
    {
        //There is enough space in the buffer for the request line.
        if ((mRequestLine.length() - mEncodingRequestLinePos) <= bufferLength)
        {
            memcpy(
                pBuffer,
                mRequestLine.data() + mEncodingRequestLinePos,
                mRequestLine.length() - mEncodingRequestLinePos );

            encodedLength = mRequestLine.length() - mEncodingRequestLinePos;

            bufferLength -= mRequestLine.length() - mEncodingRequestLinePos;

            mEncodingRequestLinePos = std::string::npos;

        }
        else
        {
            memcpy(
                pBuffer,
                mRequestLine.data() + mEncodingRequestLinePos,
                bufferLength );

            encodedLength += bufferLength;

            mEncodingRequestLinePos += bufferLength;

            bufferLength = 0;
        }
    }

    //Encode the request headers.
    while (mEncodingRequestHeaderItr != mRequestHeaders.end())
    {
        // 4 = ": " + "\r\n"
        if ((mEncodingRequestHeaderItr->first.length()
             + mEncodingRequestHeaderItr->second.length()
             + 4) <= bufferLength)
        {
            memcpy(
                pBuffer + encodedLength,
                mEncodingRequestHeaderItr->first.data(),
                mEncodingRequestHeaderItr->first.length() );

            encodedLength += mEncodingRequestHeaderItr->first.length();
            bufferLength -= mEncodingRequestHeaderItr->first.length();

            memcpy(
                pBuffer + encodedLength,
                ": ",
                2 );

            encodedLength += 2;
            bufferLength -= 2;

            memcpy(
                pBuffer + encodedLength,
                mEncodingRequestHeaderItr->second.data(),
                mEncodingRequestHeaderItr->second.length() );

            encodedLength += mEncodingRequestHeaderItr->second.length();
            bufferLength -= mEncodingRequestHeaderItr->second.length();

            memcpy(
                pBuffer + encodedLength,
                "\r\n",
                2 );

            encodedLength += 2;
            bufferLength -= 2;

            mEncodingRequestHeaderItr++;
        }
        else
        {
            break;
        }
    }

    if ((mEncodingRequestHeaderItr == mRequestHeaders.end())
        && (mState == SENDING_LINE_HEADERS_STATE)
        && (!mRequestHeadersEndEncoded)
        && (2 <= bufferLength))
    {
        memcpy(
            pBuffer + encodedLength,
            "\r\n",
            2 );

        encodedLength += 2;

        mRequestHeadersEndEncoded = true;
    }

    bufferLength = encodedLength;
}

/** Set the response code. */
void AsyncHttpSocketRequest::setResponseStatus(
                                    const std::string& httpVersion,
                                    const std::string& code,
                                    const std::string& /*reasonPhrase*/ )
{
    //Parse the version string and get major and minor number.
    mResponseHttpVersionMajor = 0;
    mResponseHttpVersionMinor = 0;

    std::string::size_type dotPos = httpVersion.find( "." );

    if (dotPos != std::string::npos && dotPos > 0)
    {
        mResponseHttpVersionMajor = strtoul(httpVersion.c_str() + dotPos - 1, NULL, 10 );
        mResponseHttpVersionMinor = strtoul(httpVersion.c_str() + dotPos + 1, NULL, 10 );
    }

    //Work out the response code.
    mResponseCode = strtoul( code.c_str(), NULL, 10 );

    //Set the responsebody length based on the request method and certain response codes.
    if ((mMethod == HEAD_METHOD)
        || (mResponseCode >= 100 && mResponseCode <= 199)
        || (mResponseCode == 204)
        || (mResponseCode == 304))
    {
        mResponseBodyLength = 0;
    }
}

/** Add a response header name. */
void AsyncHttpSocketRequest::addResponseHeaderName(const unsigned char *pNameData,
                                                   uint32_t nameLength)
{
    assert( mAddingResponseHeaderItr == mResponseHeaders.end() );

    // Trim the white spaces in the end.
    while ((nameLength > 0)
           && ((pNameData[nameLength - 1] == ' ') || (pNameData[nameLength - 1] == '\t'))) {
        nameLength--;
    }

    const std::string headerName(reinterpret_cast<const char *>(pNameData), nameLength);

    mAddingResponseHeaderItr = mResponseHeaders.insert(mResponseHeaders.end(),
                                                       std::make_pair(headerName, std::string()));
}

/** Add a response header value. */
void AsyncHttpSocketRequest::addResponseHeaderValue(
                                    const unsigned char* pValueData,
                                    uint32_t valueLength )
{
    assert( mAddingResponseHeaderItr != mResponseHeaders.end() );

    //Trim the white spaces in the end.
    while ((valueLength > 0)
            && ((pValueData[valueLength - 1] == ' ') || (pValueData[valueLength - 1] == '\t')))
    {
        valueLength--;
    }

    if (valueLength > 0)
    {
        if (!mAddingResponseHeaderItr->value.empty())
        {
            mAddingResponseHeaderItr->value.append( " " );
        }

        mAddingResponseHeaderItr->value.append(
                                    std::string(
                                            (const char*) pValueData,
                                            valueLength ) );
    }
}

/** Signal that a response header has been fully received and parsed. */
int32_t AsyncHttpSocketRequest::signalResponseHeaderCompleted()
{
    int32_t retVal = AS_NO_ERROR;
    if (mMethod != HEAD_METHOD)
    {
        //Take a look at the headers that we are interested.
        if (strcasecmp( mAddingResponseHeaderItr->header.c_str(), "Content-Length" ) == 0)
        {
            if ((mResponseCode >= 100 && mResponseCode <=199)
                || (mResponseCode == 204)
                || (mResponseCode == 304))
            {
                mResponseBodyLength = 0;
            }
            else if (mChunkedTransferFlag)
            {
                mResponseBodyLength = -1;
            }
            else
            {
                mResponseBodyLength = strtoll(
                                    mAddingResponseHeaderItr->value.c_str(),
                                    NULL,
                                    10 );
                if (mMaxContentLength && mResponseBodyLength > mMaxContentLength)
                    retVal = AS_HTTP_MESSAGE_LENGTH_ERROR;
            }
        }
        else if (strcasecmp( mAddingResponseHeaderItr->header.c_str(), "Transfer-Encoding" ) == 0)
        {
            if (strcasecmp( mAddingResponseHeaderItr->value.c_str(), "chunked" ) == 0)
            {
                mChunkedTransferFlag = true;

                mResponseBodyLength = -1;
            }
        }
    }
    else
    {
        mResponseBodyLength = 0;
    }

    mAddingResponseHeaderItr = mResponseHeaders.end();
    return retVal;
}


/** Set the response body to size to be zero for the case where this is not clear from the header */
void AsyncHttpSocketRequest::setResponseBodyEmpty()
{
    mResponseBodyLength = 0;
}

/** Add staged response body. */
void AsyncHttpSocketRequest::addStagedBody(
                                    const unsigned char* pStagedBodyData,
                                    uint32_t stagedBodyLength )
{
    mStagedBodyDataPtr = pStagedBodyData;
    mStagedBodyLength = stagedBodyLength;


    IHttpRequestTraceListenerPtr const pListener = mTraceListenerPtr.lock();
    if (pListener)
    {
        pListener->reportBytesReceived(
                                    AseTimeVal::now(),
                                    stagedBodyLength,
                                    false,
                                    stagedBodyLength == mResponseBodyLength );
    }
}

/** Added staged chunks. */
void AsyncHttpSocketRequest::addStagedChunk(
                                    const unsigned char* pStagedChunk,
                                    uint32_t stagedChunkLength,
                                    uint32_t stagedChunkSize )
{
    //Check if the staged chunk is complete.
    if (stagedChunkLength == stagedChunkSize)
    {
        if (mStagedCompletedChunkList.empty())
        {
            mStagedCompletedChunkList.push_back( std::make_pair( pStagedChunk, stagedChunkLength ) );

            mCopyingStagedChunkItr = mStagedCompletedChunkList.begin();

            mCopiedChunkLength = 0;
        }
        else
        {
            mStagedCompletedChunkList.push_back( std::make_pair( pStagedChunk, stagedChunkLength ) );
        }
    }
    else
    {
        mStagedPartialChunkPtr = pStagedChunk;
        mStagedPartialChunkLength = stagedChunkLength;
        mStagedPartialChunkSize = stagedChunkSize;
    }

    IHttpRequestTraceListenerPtr const pListener = mTraceListenerPtr.lock();
    if (pListener)
    {
        pListener->reportBytesReceived(
                                    AseTimeVal::now(),
                                    stagedChunkLength,
                                    false,
                                    false );
    }
}

/** Get the staged body length. */
bool AsyncHttpSocketRequest::hasStagedResponseBody()
{
    bool retVal = false;

    if (!mChunkedTransferFlag)
    {
        if (mStagedBodyLength != 0)
        {
            retVal = true;
        }
    }
    else
    {
        if (!mStagedCompletedChunkList.empty() || (mStagedPartialChunkLength != 0))
        {
            retVal = true;
        }
    }

    return retVal;
}

/** Set the chunked transfer state. */
void AsyncHttpSocketRequest::setChunkedTransferState( ChunkedTransferState state )
{
    mChunkedTransferState = state;
}

/** Get the chunked transfer state. */
AsyncHttpSocketRequest::ChunkedTransferState AsyncHttpSocketRequest::getChunkedTransferState() const
{
    return mChunkedTransferState;
}

/** Receive the regular response body. */
int32_t AsyncHttpSocketRequest::receiveRegularResponseBody(
                                            unsigned char* pBuffer,
                                            uint32_t& bufferLength )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    if (mState == IAsyncHttpRequest::COMPLETED_STATE)
    {
        //Check if there is any staged data.
        if (mStagedBodyLength > 0)
        {
            if (mStagedBodyLength <= bufferLength)
            {
                memcpy( pBuffer, mStagedBodyDataPtr, mStagedBodyLength );

                bufferLength = mStagedBodyLength;

                mReceivedResponseBodyLength += mStagedBodyLength;

                mStagedBodyLength = 0;

                mAsyncHttpSocketConnection->signalResponseBodyCompleted( this );

                retVal = 1;
            }
            else
            {
                memcpy( pBuffer, mStagedBodyDataPtr, bufferLength );

                mStagedBodyDataPtr += bufferLength;
                mStagedBodyLength -= bufferLength;

                mReceivedResponseBodyLength += bufferLength;

                retVal = AS_NO_ERROR;
            }
        }
        else
        {
            bufferLength = 0;

            retVal = 1;
        }
    }
    else if (mState == IAsyncHttpRequest::RECEIVING_BODY_STATE)
    {
        //Check if there is any staged data.
        if (mStagedBodyLength > 0)
        {
            if (mStagedBodyLength <= bufferLength)
            {
                memcpy( pBuffer, mStagedBodyDataPtr, mStagedBodyLength );

                bufferLength = mStagedBodyLength;

                mReceivedResponseBodyLength += mStagedBodyLength;

                mStagedBodyLength = 0;

                retVal = AS_NO_ERROR;
            }
            else
            {
                memcpy( pBuffer, mStagedBodyDataPtr, bufferLength );

                mStagedBodyDataPtr += bufferLength;
                mStagedBodyLength -= bufferLength;

                mReceivedResponseBodyLength += bufferLength;

                retVal = AS_NO_ERROR;
            }
        }
        else
        {
            assert( mResponseBodyLength < 0 || mReceivedResponseBodyLength < mResponseBodyLength );

            //Receive the response body directly from the socket.
            llong dataToReceive;
            if (mResponseBodyLength < 0) {
                dataToReceive = bufferLength;
            } else {
                dataToReceive = mResponseBodyLength - mReceivedResponseBodyLength;
            }

            if (dataToReceive > bufferLength)
            {
                dataToReceive = bufferLength;
            }

            retVal = mAsyncHttpSocketConnection->receive(
                                                    pBuffer,
                                                    (uint32_t) dataToReceive,
                                                    0 );

            if (retVal > 0)
            {
                bufferLength = retVal;

                mReceivedResponseBodyLength += bufferLength;

                bool const complete = ( mReceivedResponseBodyLength == mResponseBodyLength );

                IHttpRequestTraceListenerPtr const pListener = mTraceListenerPtr.lock();
                if (pListener)
                {
                    pListener->reportBytesReceived(
                                                        AseTimeVal::now(),
                                                        bufferLength,
                                                        false,
                                                        complete );
                }

                if (!complete)
                {
                    retVal = AS_NO_ERROR;
                }
                else
                {
                    mState = IAsyncHttpRequest::COMPLETED_STATE;

                    mAsyncHttpSocketConnection->signalResponseBodyCompleted( this );

                    retVal = 1;
                }
            }
            else if (retVal == 0)
            {
                if (mResponseBodyLength < 0) {
                    mState = IAsyncHttpRequest::COMPLETED_STATE;

                    mAsyncHttpSocketConnection->signalResponseBodyCompleted( this );
                    bufferLength = 0;

                    retVal = 1;
                } else {
                    mAsyncHttpSocketConnection->signalConnectionFailure(
                        AS_CONNECTION_CLOSED,
                        -1 );

                    retVal = AS_CONNECTION_CLOSED;
                }
            }
            else
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    bufferLength = 0;

                    retVal = AS_NO_ERROR;
                }
                else
                {
                    if (errno == ECONNRESET)
                    {
                        retVal = AS_CONNECTION_RESET_WHILE_RECEIVING;
                    }
                    else
                    {
                        retVal = AS_CONNECTION_ERROR;
                    }

                    mAsyncHttpSocketConnection->signalConnectionFailure(
                                                                    retVal,
                                                                    errno );
                }
            }
        }
    }

    return retVal;
}

/** Receive the chunked response body. */
int32_t AsyncHttpSocketRequest::receiveChunkedResponseBody(
                                            unsigned char* pBuffer,
                                            uint32_t& bufferLength )
{
    int32_t retVal = AS_UNKNOWN_ERROR;
    uint32_t copiedLength = 0;
    uint32_t dataToCopy;

    if (mState == IAsyncHttpRequest::COMPLETED_STATE)
    {
        //Copy the staged completed chunks to the buffer.
        while ((mCopyingStagedChunkItr != mStagedCompletedChunkList.end())
               && (bufferLength > 0))
        {
            dataToCopy = bufferLength;

            if (dataToCopy > (mCopyingStagedChunkItr->second - mCopiedChunkLength))
            {
                dataToCopy = mCopyingStagedChunkItr->second - mCopiedChunkLength;
            }

            memcpy(
                pBuffer + copiedLength,
                mCopyingStagedChunkItr->first + mCopiedChunkLength,
                dataToCopy );

            copiedLength += dataToCopy;

            bufferLength -= dataToCopy;

            mCopiedChunkLength += dataToCopy;

            mReceivedResponseBodyLength += dataToCopy;

            if (mCopiedChunkLength == mCopyingStagedChunkItr->second)
            {
                mStagedCompletedChunkList.pop_front();

                mCopyingStagedChunkItr = mStagedCompletedChunkList.begin();

                mCopiedChunkLength = 0;
            }
        }

        bufferLength = copiedLength;

        if (mStagedCompletedChunkList.empty())
        {
            if (mReceivedResponseBodyLength > 0)
            {
                mAsyncHttpSocketConnection->signalResponseBodyCompleted( this );
            }

            retVal = 1;
        }
        else
        {
            retVal = AS_NO_ERROR;
        }
    }
    else if (mState == IAsyncHttpRequest::RECEIVING_BODY_STATE)
    {
        if (!mStagedCompletedChunkList.empty() || (mStagedPartialChunkLength > 0))
        {
            //Copy the staged completed chunks to the buffer.
            while ((mCopyingStagedChunkItr != mStagedCompletedChunkList.end())
                   && (bufferLength > 0))
            {
                dataToCopy = bufferLength;

                if (dataToCopy > (mCopyingStagedChunkItr->second - mCopiedChunkLength))
                {
                    dataToCopy = mCopyingStagedChunkItr->second - mCopiedChunkLength;
                }

                memcpy(
                    pBuffer + copiedLength,
                    mCopyingStagedChunkItr->first + mCopiedChunkLength,
                    dataToCopy );

                copiedLength += dataToCopy;

                bufferLength -= dataToCopy;

                mCopiedChunkLength += dataToCopy;

                mReceivedResponseBodyLength += dataToCopy;

                if (mCopiedChunkLength == mCopyingStagedChunkItr->second)
                {
                    mStagedCompletedChunkList.pop_front();

                    mCopyingStagedChunkItr = mStagedCompletedChunkList.begin();

                    mCopiedChunkLength = 0;
                }
            }

            if (mStagedCompletedChunkList.empty())
            {
                dataToCopy = bufferLength;

                if (dataToCopy > mStagedPartialChunkLength)
                {
                    dataToCopy = mStagedPartialChunkLength;
                }

                memcpy(
                    pBuffer + copiedLength,
                    mStagedPartialChunkPtr,
                    dataToCopy );

                copiedLength += dataToCopy;

                mStagedPartialChunkPtr += dataToCopy;

                bufferLength -= dataToCopy;

                mStagedPartialChunkLength -= dataToCopy;

                mStagedPartialChunkSize -= dataToCopy;

                mReceivedResponseBodyLength += dataToCopy;
            }

            bufferLength = copiedLength;

            retVal = AS_NO_ERROR;
        }
        else if ( mStagedPartialChunkSize > 0 )
        {
            //Receive the response body directly from the socket.
            uint32_t dataToReceive = mStagedPartialChunkSize;

            if (dataToReceive > bufferLength)
            {
                dataToReceive = bufferLength;
            }

            retVal = mAsyncHttpSocketConnection->receive(
                                                    pBuffer,
                                                    dataToReceive,
                                                    0 );

            if (retVal > 0)
            {
                bufferLength = retVal;

                IHttpRequestTraceListenerPtr const pListener = mTraceListenerPtr.lock();
                if (pListener)
                {
                    pListener->reportBytesReceived(
                                                AseTimeVal::now(),
                                                bufferLength,
                                                false,
                                                false );
                }

                mStagedPartialChunkSize -= retVal;

                if (mStagedPartialChunkSize == 0)
                {
                    mChunkedTransferState = RECEIVING_CHUNK_DATA_END_STATE;

                    mStagedCompletedChunkList.clear();

                    mCopyingStagedChunkItr = mStagedCompletedChunkList.end();
                    mCopiedChunkLength = 0;
                    mStagedPartialChunkPtr = NULL;
                    mStagedPartialChunkLength = 0;
                }

                retVal = AS_NO_ERROR;
            }
            else if (retVal == 0)
            {
                mAsyncHttpSocketConnection->signalConnectionFailure(
                                                    AS_CONNECTION_CLOSED,
                                                    -1 );

                retVal = AS_CONNECTION_CLOSED;
            }
            else
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    bufferLength = 0;

                    retVal = AS_NO_ERROR;
                }
                else
                {
                    if (errno == ECONNRESET)
                    {
                        retVal = AS_CONNECTION_RESET;
                    }
                    else
                    {
                        retVal = AS_CONNECTION_ERROR;
                    }

                    mAsyncHttpSocketConnection->signalConnectionFailure(
                                                            retVal,
                                                            errno );
                }
            }
        }
        else
        {
          // There are no staged complete chunks and no staged partial chunk data
          // and we have received all of any previous staged partial chunk
          // So, we have nothing to return
          bufferLength = 0;

          retVal = AS_NO_ERROR;
        }
    }

    return retVal;
}

/** Indicate bytes received */
void AsyncHttpSocketRequest::indicateHeaderBytesReceived(
                                        AseTimeVal const& time,
                                        uint32_t bytes,
                                        bool complete )
{
    IHttpRequestTraceListenerPtr const pListener = mTraceListenerPtr.lock();
    if (bytes > 0 && (pListener))
    {
        pListener->reportBytesReceived(
                                            time,
                                            bytes,
                                            true,
                                            complete );
    }
}

/** Indicate receiving is paused on this request. */
void AsyncHttpSocketRequest::pauseReceiving( const AseTimeVal& now )
{
    assert( mPausedTime == AseTimeVal::INFINITE );

    mPausedTime = now;
}

/** Indicate receiving is resumed on this request. */
void AsyncHttpSocketRequest::resumeReceiving( const AseTimeVal& now )
{
    if (now >= mPausedTime)
    {
        mTimeoutTime += (now - mPausedTime);
    }

    mPausedTime = AseTimeVal::INFINITE;
}

/** Set the timeout time. */
void AsyncHttpSocketRequest::setTimeoutTime( const AseTimeVal& now )
{
    //If the timeout time is set when the request is paused, reset the paused time,
    //so that the timeout time can be calculated correctly at resume.
    if (mPausedTime != AseTimeVal::INFINITE)
    {
        mPausedTime = now;
    }

    mTimeoutTime = now + mTimeoutDuration;
}

//Get the ID. */
uint32_t AsyncHttpSocketRequest::getId() const
{
    return mId;
}

/** Get the method. */
IAsyncHttpRequest::Method AsyncHttpSocketRequest::getMethod() const
{
    return mMethod;
}

/** Get the version. */
IAsyncHttpRequest::Version AsyncHttpSocketRequest::getVersion() const
{
    return mVersion;
}

/** Get the state */
IAsyncHttpRequest::State AsyncHttpSocketRequest::getState() const
{
    return mState;
}

/** Get the progress of the request. */
int32_t AsyncHttpSocketRequest::getProgress() const
{
    int32_t retVal;

    if (mResponseBodyLength > 0)
    {
        retVal = mReceivedResponseBodyLength * 100 / mResponseBodyLength;
    }
    else if (mResponseBodyLength == 0)
    {
        retVal = 100;
    }
    else
    {
        retVal = -1;
    }

    return retVal;
}

/** Set the user data which is some opaque data which the caller wants to associate with the request. */
void AsyncHttpSocketRequest::setUserData( void* pData )
{
    mUserDataPtr = pData;
}

/** Get the user data. */
void* AsyncHttpSocketRequest::getUserData() const
{
    return mUserDataPtr;
}

/** Get the response HTTP version, only valid when state >= RECEIVING_HEADERS_STATE. */
void AsyncHttpSocketRequest::getResponseHttpVersion( uint32_t& major, uint32_t& minor )
{
    major = mResponseHttpVersionMajor;
    minor = mResponseHttpVersionMinor;

    return;
}

/** Get the response code. */
uint32_t AsyncHttpSocketRequest::getResponseCode() const
{
    return mResponseCode;
}

/** Get the value of a particular response header. */
std::string AsyncHttpSocketRequest::getResponseHeaderValue( std::string const& name ) const
{
    HttpResponseHeaders::const_iterator headerItr = mResponseHeaders.begin();

    while (headerItr != mResponseHeaders.end()) {
        if (!strcasecmp(headerItr->header.c_str(), name.c_str()))
            break;
        ++headerItr;
    }

    if (headerItr != mResponseHeaders.end())
    {
        return headerItr->value;
    }
    else
    {
        return std::string();
    }
}

/** Get all the response headers */
HttpResponseHeaders AsyncHttpSocketRequest::getResponseHeaders() const
{
    return mResponseHeaders;
}

/** Get the chunked transfer flag. */
bool AsyncHttpSocketRequest::getChunkedTransferFlag() const
{
    return mChunkedTransferFlag;
}

/** Get the content length. */
llong AsyncHttpSocketRequest::getContentLength() const
{
    return mResponseBodyLength;
}

/** Get the response body length. */
llong AsyncHttpSocketRequest::getResponseBodyLength() const
{
    return mResponseBodyLength;
}

/** Get the response body. */
int32_t AsyncHttpSocketRequest::getResponseBody( unsigned char* pBuffer, uint32_t& bufferLength )
{
    int32_t retVal = AS_UNKNOWN_ERROR;

    if (!mChunkedTransferFlag)
    {
        retVal = receiveRegularResponseBody( pBuffer, bufferLength );
    }
    else
    {
        retVal = receiveChunkedResponseBody( pBuffer, bufferLength );
    }

    return retVal;
}

int32_t AsyncHttpSocketRequest::discardResponseBody( uint32_t& bytesDiscarded, bool const limit )
{
    uint32_t const discardBufferCapacity = 1024;
    unsigned char discardBuffer[ discardBufferCapacity ];

    uint32_t maxBytesToDiscard = bytesDiscarded;
    bytesDiscarded = 0;

    uint32_t discard;
    int32_t retVal = AS_NO_ERROR;
    do
    {
        discard = ( limit && discardBufferCapacity > maxBytesToDiscard ) ? maxBytesToDiscard
                                                                         : discardBufferCapacity;
        retVal = getResponseBody( discardBuffer, discard );
        bytesDiscarded += discard;
        maxBytesToDiscard -= discard;

    } while ((retVal == AS_NO_ERROR) && (discard > 0 ) && (!limit || maxBytesToDiscard > 0));

    return retVal;
}

/** Get the timeout time. */
const AseTimeVal& AsyncHttpSocketRequest::getTimeoutTime() const
{
    if (mPausedTime == AseTimeVal::INFINITE)
    {
        return mTimeoutTime;
    }
    else
    {
        return AseTimeVal::INFINITE;
    }
}

/** Get the time the client sends the first byte. */
const AseTimeVal& AsyncHttpSocketRequest::getFirstByteSentTime() const
{
    return mFirstByteSentTime;
}

/** Get the time the client receives the first byte. */
const AseTimeVal& AsyncHttpSocketRequest::getFirstByteReceivedTime() const
{
    return mFirstByteReceivedTime;
}

/** Validate the message length. */
bool AsyncHttpSocketRequest::validMessageLength()
{
    bool retVal = true;

    bool contentLengthFound = false;
    bool chunkedTransferFound = false;

    HttpResponseHeaders::iterator itrResponseHeader;

    itrResponseHeader = mResponseHeaders.begin();

    while (itrResponseHeader != mResponseHeaders.end())
    {
        //Take a look at the headers that we are interested.
        if (strcasecmp( itrResponseHeader->header.c_str(), "Content-Length" ) == 0)
        {
            contentLengthFound = true;
        }

        if ((strcasecmp( itrResponseHeader->header.c_str(), "Transfer-Encoding" ) == 0)
            && (strcasecmp( itrResponseHeader->value.c_str(), "chunked" ) == 0))
        {
            chunkedTransferFound = true;
        }

        itrResponseHeader++;
    }

    if ((!chunkedTransferFound) && (!contentLengthFound))
    {
        retVal = false;
    }

    return retVal;
}

}
