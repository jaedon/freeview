/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MEDIASTREAM_REQUEST_H
#define MEDIASTREAM_REQUEST_H

#include "MediaPresentationRequest.h"
#include "MediaFragment.h"

namespace netflix {
namespace ase {
/**
 * @class MediaStreamRequest
 * @brief An MediaRequest object created by the MediaStream
 *
 * This object represents an MEDIA request for media data. The request
 * object forwards data and indications to a MediaFragment object.
 *
 */
class MediaStreamRequest : public MediaPresentationRequest
{
public:
    typedef shared_ptr<MediaStreamRequest> MediaStreamRequestPtr;

    /** Construct a new MediaStreamRequest object
     *  @param urls [in] The URLs for the target file
     *  @param byteRange [in] The byte range of the request
     *  @param pMediaBufferPool [in] The media buffer pool
     *  @param pFragment [in] The fragment
     *  @param pRequest [out] The constructed object
     *  @return 0 success or negative error code to be defined
     */
    static int construct( ManifestUrls const&        urls,
                          ByteRange const&              byteRange,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          shared_ptr<IMediaFragment>      pFragment,
                          std::string const& streamKey,
                          MediaType mediaType,
                          uint32_t cdnid,
                          MediaStreamRequestPtr&    pRequest );


    // MediaRequest interface

    //virtual ManifestUrls const& getUrls( ) const;

    virtual ByteRange const& getByteRange( ) const;

    virtual AseTimeVal getCompletionDeadline() const;

    virtual size_t getBufferSize() const;

    virtual AseErrorCode getResponseBodyBuffer(AseBufferPtr& pBuffer);

    virtual int provideResponseBody(    AseBufferPtr const& pBuffer,
                                        int sequence,
                                        AseTimeVal& abandontime );

    virtual void abandoned();

    virtual void requestError( AseErrorCode status, uint32_t httpCode );

    /** Indicate that the file size has been determined */
    virtual void provideFileSize( ullong size );

    /** Return true if this request has a URL assigned */
    virtual bool hasUrl() const ;

    /** Select the URL and return true if it has changed */
    virtual bool selectUrl();

    /** Get the URL for this request */
    virtual AseUrl getCurrentUrl();

public:
    virtual ~MediaStreamRequest() {}

protected:
    MediaStreamRequest(   ManifestUrls const&        urls,
                          ByteRange const&              byteRange,
                          IMediaBufferPoolPtr const&   pMediaBufferPool,
                          shared_ptr<IMediaFragment>    pFragment,
                          std::string const& streamKey,
                          MediaType mediaType,
                          uint32_t cdnid);


    /** The media fragment to which the data should be forwarded */
    shared_ptr<IMediaFragment>          mMediaFragmentPtr;

    IMediaBufferPoolPtr                 mMediaBufferPoolPtr;

    ManifestUrls                        mUrls;

    ByteRange                           mByteRange;

    std::string                         mStreamKey;

    MediaType                           mMediaType;

    uint32_t                            mCdnId;

    ConstManifestUrlPtr                 mCurrentUrl;

    shared_ptr<IStreamingLocationSet>   mStreamingLocationSetPtr;
};

typedef MediaStreamRequest::MediaStreamRequestPtr MediaStreamRequestPtr;

}}
#endif

