/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ISO_MEDIA_FILE_H
#define ISO_MEDIA_FILE_H

#include <nrdase/AseCommonDataTypes.h>
#include "Range.h"

#include <nrdase/IMediaFile.h>
#include <nrdase/IAseConfigParameter.h>
#include "HeapBuffer.h"
#include "BoxReader.h"
#include "IsoMediaFileRequest.h"
#include "Range.h"
#include <nrdase/DebugLogging.h>

#include <queue>

namespace netflix {
namespace ase {

DECLARE_LOGGER( IsoMediaFile );

/** Class for handling ISO Base Media Files
 *
 *  This class (and subclasses) manage the header (moov box) and index
 *  and possibly other initial boxes that are needed by the streaming
 *  engine.
 *
 *  Initial boxes are of two types: those that need to be searched
 *  for by stepping box by box through the file (up to the first
 *  moof) and those whose byte range is known.
 *
 *  The class goes through several states
 *      - searching for initial boxes
 *      - downloading boxes with known positions
 *      - initial download complete
 *
 *  The types and locations (if known) of the initial boxes, except the ftyp
 *  and moov) must be provided by subclasses.
 *
 *  ISO Media Files carry a "brand" identification in the ftyp box. This determine
 *  the file type (more precisely than the MIME type).
 *
 *  Subclasses should register "brand handlers" which should in turn register
 *  box handlers for the boxes to be expected in files of that brand.
 *
 *  A file may be compatible to multiple brands. In this case there  may be
 *  conflict as to which box should fulfil a certain function (particularly
 *  the index). This is resolved when calling brand handlers: they are
 *  called in the order they were registered and can claim responsibility for the
 *  index functions.
 */



class IsoMediaFile : public IMediaFile
{
public:
    friend class IsoMediaFileRequest;
    friend struct FtypBoxHandler;
    friend struct FreeBoxHandler;
    friend struct MoovBoxHandler;

    typedef shared_ptr<IsoMediaFile> IsoMediaFilePtr;

    // Minimum box size: 4-byte size + 4-byte type.
    static ullong const sMinimumBoxSize = 8;

    // Maximum size for header boxes, including unrecognised ones
    static ullong const sMaximumHeaderBoxSize = 10485760;

    // Maximum size for supported header boxes
    static ullong const sMaximumSupportedHeaderBoxSize = 1048576;

    struct BrandHandler
    {
        virtual AseErrorCode operator()( IsoMediaFilePtr pIsoMediaFile, bool& index ) = 0;
        virtual ~BrandHandler() {}
    };

    struct BoxHandler
    {
        /** Called to initiate handling of a new box
         *  @param [in] pIsoMediaFile The IsoMediaFile object
         *  @param [in] reader BoxReader with header already read
         *  @param [in] pBoxBuffer Buffer containing box data.
         */
        virtual AseErrorCode init(  IsoMediaFilePtr     pIsoMediaFile,
                                    BoxReader const&    reader ) = 0;

        /** Called when additional bytes are to be added to the box */
        virtual AseErrorCode process( uint8_t const *pData, size_t size ) = 0;

        virtual ~BoxHandler() {}
    };

    class BufferedBoxHandler : public BoxHandler
    {
    public:
        virtual AseErrorCode init(  IsoMediaFilePtr     pIsoMediaFile,
                                    BoxReader const&    reader );

        virtual AseErrorCode process( uint8_t const *pData, size_t size );

        virtual ~BufferedBoxHandler() {}

    protected:
        /** Called each time data is added to the box */
        virtual AseErrorCode doProcess() = 0;

    protected:
        weak_ptr<IsoMediaFile>  mIsoMediaFilePtr;
        HeapBufferPtr           mBufferedBoxPtr;
        ullong                mBoxOffset;
    };

    typedef shared_ptr<BoxHandler> BoxHandlerPtr;

    struct BoxHandlerFactory
    {
        /** Create a new box handler */
        virtual BoxHandlerPtr createBoxHandler( BoxReader const& reader ) = 0;

        virtual ~BoxHandlerFactory() {}
    };

    // IsoMediaFile

    void registerBrand( fourcc_t brand, BrandHandler& handler );

    void registerDefaultBrand( BrandHandler& handler );

    void registerBox( fourcc_t box, BoxHandlerFactory& factory );

    void registerBox( nf_uuid_t const& box, BoxHandlerFactory& factory );

    /** Indicate that the base class should seatch for a box
     *
     *  Subclasses should call this for every box they need, as soon as possible
     */
    void setBoxSearch( fourcc_t box );

    void setBoxSearch( nf_uuid_t const& box );

    /** Indicate that searching for a box is no longer needed */
    void clearBoxSearch( fourcc_t box );

    void clearBoxSearch( nf_uuid_t const& box );

    // IMediaConsumer
    /** Get a new Request object
     *  @param pRequest [out] The new MediaRequest object
     *  @return 0   for success, negative error code, or positive number
     *              indicating minimum time before next call in ms
     */
    virtual AseErrorCode getNextRequest( MediaPresentationRequestPtr& pRequest );

    // IMediaFile

    /** Test if the MediaFile is in a good state */
    virtual bool good() const;

    /** Test if the MediaFile has the media duration */
    virtual bool hasDuration() const;

    /** Test if the MediaFile object has the header */
    virtual bool hasHeader() const;

    /** Test if the MediaFile object has (any of) the index */
    virtual bool hasIndex() const;

    /** Test if the index is complete */
    virtual bool indexComplete() const;

    /** Get the content duration */
    virtual bool getDuration( AseTimeInterval& duration ) const;

    /** Get the fragment duration in ms */
    virtual bool getFragmentDuration( AseTimeInterval& fragmentDuration ) const;

    /** Get the byte range of the header+index */
    virtual ByteRange getHeaderRange() const;

    /** Provide a byte range hint for the header+ndex */
    virtual void provideHeaderRangeHint( ByteRange const& range );

    /** Get the media header
     * @param [out] media header buffer
     * @return True if the header was returned
     */
    virtual bool getHeader( AseBufferPtr& mediaHeader ) const;

    /** query the media index given the time range
     * @param [out] pIndex The index vector
     * @param [out] indexStart start time of the first fragment
     * @param [out] indexEnd end of the last fragment in the index
     * @param [out] mediaEnd end time of the last fragment in the media
     * @param [out] mediaOffset the byte offset of the first fragment
     * @return true if index information was returned, false if not yet available
     */
    virtual bool getIndex(  MediaFileIndexConstPtr& pIndex,
                            AseTimeStamp& indexStart,
                            AseTimeStamp& indexEnd,
                            ullong& mediaOffset ) const;

    /** Obtain the last error if getHeader or getIndex returned FAILED
    * @return the error code
    */
    virtual AseErrorCode getLastError() const;

    virtual std::string const& getStreamKey() const;

    /** Get a idividualization token from the file */
    virtual std::vector<unsigned char> const& getFileToken() const;

    virtual void reset();

public:
    virtual ~IsoMediaFile() {}

protected:

    IsoMediaFile( ConstManifestStreamPtr pManifestSteam );

    /** Ask if the object has MEDIA requests it wishes to issue
     *  @return True if the object has requests to issue. The caller
     *               should call getNextRequest to get the next request
     */
    bool doesNeedNewRequest() const;

    AseErrorCode initialize( IAseConfigParameterPtr pConfigParameters );

    virtual void provideFileSize( ullong fileSize );

    virtual void provideDuration( AseTimeInterval dur );

    int provideResponseBody(    IsoMediaFileRequestPtr  pRequest,
                                AseBufferPtr            pBuffer,
                                ullong                offset,
                                bool                    bRequestComplete );

    void provideFailureCode( AseErrorCode code );

    AseErrorCode handleNewBoxes(    AseBufferPtr    pBuffer,
                                    ullong        offset );

    int searchBoxes() const;

    void clearBoxSearch( BoxReader const& reader );

    AseErrorCode createNewRequest( Range<llong> const& range, MediaPresentationRequestPtr& pRequest );

    void requestByteRange( ByteRange const& range );

    void ignoreByteRange( ByteRange const& range );

    AseErrorCode handleFtypBox( BoxReader const& reader );

    AseErrorCode handleFreeBox( BoxReader const& reader );

    AseErrorCode handleMoovBox( AseBufferPtr pBuffer );

    BoxHandlerPtr createBoxHandler( BoxReader const& reader );

protected:
    typedef enum
    {
        BOX_PROCESSING_INIT = 0,
        BOX_PROCESSING_HEADER = 1,
        BOX_PROCESSING_BOX = 2,
        BOX_PROCESSING_IGNORED_BOX = 3
    } BoxProcessingState;

protected:

    typedef std::vector<IsoMediaFileRequestPtr>     RequestsQueue;
    typedef std::pair<fourcc_t,BrandHandler*>       BrandHandlerEntry;
    typedef std::pair<fourcc_t,BoxHandlerFactory*>  BoxHandlerFactorEntry;
    typedef std::pair<nf_uuid_t,BoxHandlerFactory*> ExtendedBoxHandlerFactorEntry;

    std::vector<BrandHandlerEntry>              mBrandHandlers;

    std::vector<BrandHandler*>                  mDefaultBrandHandlers;

    std::vector<BoxHandlerFactorEntry>          mBoxHandlers;

    std::vector<ExtendedBoxHandlerFactorEntry>  mExtendedBoxHandlers;

    llong                     mInitialRequestSize;

    ByteRange                   mInitialRequestHint;

    ConstManifestStreamPtr      mManifestStreamPtr;

    std::vector<fourcc_t>       mRequiredBoxes;

    std::vector<nf_uuid_t>      mRequiredUuidBoxes;

    Ranges<llong>             mRequiredByteRanges;

    Ranges<llong>             mDoneByteRanges;

    Ranges<llong>             mIgnoredByteRanges;

    IsoMediaFileRequestPtr      mCurrentRequestPtr;

    BoxProcessingState          mBoxProcessingState;

    ullong                    mCurrentBoxOffset;

    AseBufferPtr                mCurrentBoxBufferPtr;

    BoxHandlerPtr               mCurrentBoxHandlerPtr;

    size_t                      mRemainingBytes;

    AseErrorCode                mFailure;

    ullong                    mFileSize;

    AseTimeInterval             mDuration;

    AseTimeInterval             mFragmentDuration;

    AseBufferPtr                mHeaderPtr;

    AseTimeInterval             mIndexStart;

    AseTimeInterval             mIndexEnd;

    ullong                    mMovieFragmentsOffset;

    MediaFileIndexPtr           mIndexPtr;

    bool                        mbIndexComplete;

    bool                        mbParseMoovBox;

    std::vector<unsigned char>  mFreeToken;

};

typedef IsoMediaFile::IsoMediaFilePtr IsoMediaFilePtr;

}}
#endif
