/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BUFFERED_MEDIA_REQUEST_H
#define BUFFERED_MEDIA_REQUEST_H

#include <nrdase/IMediaConsumer.h>

#include "IJsBridgeMediaRequestListener.h"
#include "JsBridgeMediaRequest2.h"

#include <nrdase/AseCommonDataTypes.h>
#include <nrdase/IMediaBufferPool.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdnet/AseTimeVal.h>
#include <nrddemux/Reader.h>

namespace netflix {
namespace mediacontrol {

/**
 * @class JsBridgeMediaRequest
 * @brief An MediaRequest object for MediaRequestBridge
 *
 */
class BufferedMediaRequest : public JsBridgeMediaRequest
{
public:
    typedef shared_ptr<BufferedMediaRequest> BufferedMediaRequestPtr;

    /** Construct a new MediaStreamRequest object
     *  @param url [in] the url for the media request
     *  @param byteRange [in] The byte range of the request
     *  @param pMediaBufferPool [in] The media buffer pool
     *  @param mediaType
     */
    BufferedMediaRequest(   AseUrl const&               url,
                            ase::ByteRange const&       byteRange,
                            uint32_t                    progressInterval = 4096 );

    // MediaRequest interfaces
    virtual size_t                  getBufferSize() const;
    virtual AseErrorCode            getResponseBodyBuffer(ase::AseBufferPtr& pBuffer);
    virtual int                     provideResponseBody(    ase::AseBufferPtr const& pBuffer,
                                                            int sequence,
                                                            AseTimeVal& abandontime );

    virtual ~BufferedMediaRequest() {}

    virtual size_t getProgressInterval() const { return mProgressInterval; }

    virtual void reportProgress( size_t bytesReceived );

protected:
    size_t const                    mProgressInterval;

    DataBuffer                      mDataBuffer;
};


}}
#endif

