/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef APPENDABLE_MEDIA_REQUEST_H
#define APPENDABLE_MEDIA_REQUEST_H

#include "JsBridgeMediaRequest2.h"
#include "MediaSourceAppendable.h"

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
class AppendableMediaRequest : public JsBridgeMediaRequest
                             , public MediaSourceAppendable
{
public:
    typedef shared_ptr<AppendableMediaRequest> AppendableMediaRequestPtr;

    /** Construct a new MediaStreamRequest object
     *  @param url [in] the url for the media request
     *  @param byteRange [in] The byte range of the request
     *  @param pMediaBufferPool [in] The media buffer pool
     *  @param mediaType
     */
    AppendableMediaRequest( AseUrl const&               url,
                            ase::ByteRange const&       byteRange,
                            ase::IMediaBufferPoolPtr    pMediaBufferPool,
                            ase::MediaType              mediaType);

    // JsBridgeMediaRequest
    virtual ase::MediaType          getMediaBufferType() const;

    virtual bool                    isAppendable() const { return true; }

    /** Indicates that media buffer space has been reserved for this request */
    virtual void                    bufferReserved();

    // MediaRequest interfaces
    virtual size_t                  getBufferSize() const;
    virtual AseErrorCode            getResponseBodyBuffer(ase::AseBufferPtr& pBuffer);
    virtual int                     provideResponseBody(    ase::AseBufferPtr const& pBuffer,
                                                            int sequence,
                                                            AseTimeVal& abandontime );

    // MediaSourceAppendable
    virtual bool                    forwardBlocks();

public:
    virtual ~AppendableMediaRequest();

    static uint32_t computeNumberOfBlocks(uint32_t fragmentSize, uint32_t bufferUnitSize);

protected:
    ase::IMediaBufferPoolPtr const  mMediaBufferPoolPtr;

    ase::MediaType const            mMediaType;

    std::vector<ase::AseBufferPtr>  mData;

    std::vector<ase::AseBufferPtr>::size_type mNextBlock;

    uint32_t                        mReservedUnits;
};

typedef AppendableMediaRequest::AppendableMediaRequestPtr AppendableMediaRequestPtr;

inline uint32_t AppendableMediaRequest::computeNumberOfBlocks(uint32_t requestSize, uint32_t bufferUnitSize)
{
    return ( requestSize + bufferUnitSize - 1 ) / bufferUnitSize;
}

}}
#endif

