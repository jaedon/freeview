/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ISO_MEDIA_FILE_REQUEST_H
#define ISO_MEDIA_FILE_REQUEST_H

#include <nrdase/AseCommonDataTypes.h>
#include "MediaPresentationRequest.h"
#include "HeapBuffer.h"
#include <nrdase/Manifest.h>
#include <nrdase/ByteRange.h>

namespace netflix {
namespace ase {

class IsoMediaFile;
typedef shared_ptr<IsoMediaFile> IsoMediaFilePtr;

class IStreamingLocationSet;

/**
 * @class IsoMediaFileRequest
 * @brief An MediaRequest object created by the IsoMediaFile
 *
 */
class IsoMediaFileRequest : public MediaPresentationRequest
                          , public enable_shared_from_this<IsoMediaFileRequest>
{
public:
    typedef shared_ptr<IsoMediaFileRequest> IsoMediaFileRequestPtr;

    static const size_t sBufferSize = 4096;

    /** Construct a new IsoMediaFileRequest object
     *  @param[in] mediaFile The IsoMediaFile object to forward response to
     *  @param[in] byteRange The byte range required
     *  @param[in] prefixData Data already received for this byte range
     *  @param[in] bIncremental Whether data should be forwarded incrementally
     *  @param[out] pRequest The constructed object
     *  @return 0 success or negative error code to be defined
     */
    static AseErrorCode construct(  IsoMediaFilePtr         pMediaFile,
                                    ByteRange               byteRange,
                                    IsoMediaFileRequestPtr& pRequest );

    /** Indicate that the response to the request should be ignored */
    void ignore();

    /** Get the byte range that has not yet been delivered to the IsoMediaFile */
    ByteRange getUndeliveredByteRange() const;

    // MediaRequest interface

    //virtual ManifestUrls const& getUrls( ) const;

    virtual ByteRange const& getByteRange( ) const;

    virtual size_t getBufferSize() const;

    virtual AseErrorCode getResponseBodyBuffer( AseBufferPtr& pBuffer );

    virtual int provideResponseBody( AseBufferPtr const& pBuffer, int sequence, AseTimeVal& abandontime );

    virtual void abandoned();

    virtual void requestError( AseErrorCode status, uint32_t httpCode );

    virtual void provideFileSize( ullong size );

    /** Return true if this request has a URL assigned */
    virtual bool hasUrl() const ;

    /** Select the URL and return true if it has changed */
    virtual bool selectUrl();

    /** Get the URL for this request */
    virtual AseUrl getCurrentUrl();

protected:
    IsoMediaFileRequest(    IsoMediaFilePtr         pMediaFile,
                            ByteRange               byteRange );

public:
    virtual ~IsoMediaFileRequest() {}

protected:
    weak_ptr<IsoMediaFile>              mIsoMediaFilePtr;

    ByteRange                           mByteRange;

    std::vector<AseBufferPtr>           mResponseBuffers;

    int                                 mDelivered;

    bool                                mbIgnore;

    ConstManifestUrlPtr                 mCurrentUrl;

    shared_ptr<IStreamingLocationSet>   mStreamingLocationSetPtr;
};

typedef IsoMediaFileRequest::IsoMediaFileRequestPtr IsoMediaFileRequestPtr;

}}
#endif

